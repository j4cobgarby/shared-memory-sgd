#include "NetworkExecutor.h"
#include "NetworkTopology.h"
#include "ParameterContainer.h"
#include "Optimizer.h"
#include <cassert>
#include <cmath>
#include <limits>
#include <barrier>

// #define STANDARD_WINDOW
// #define EXTEND_WINDOW
// #define SHIFT_WINDOW
#define PROBE_WHOLE
// #define SEARCH_PROBING
// #define SYNC_THREADS
// #define ALL_THREADS_MUST_FINISH

void copy_opts_vec(std::vector<MiniDNN::Optimizer *> &from, std::vector<MiniDNN::Optimizer *> &to) {
    assert(from.size() == to.size());

    for (int i = 0; i < from.size(); i++) {
        to.at(i) = from.at(i)->clone();
    }
}

void copy_nets_vec(std::vector<MiniDNN::NetworkTopology *> &from, std::vector<MiniDNN::NetworkTopology *> &to) {
    assert(from.size() == to.size());

    for (int i = 0; i < from.size(); i++) {
        to.at(i) = new MiniDNN::NetworkTopology(*from.at(i));
    }
}

void MiniDNN::NetworkExecutor::run_elastic_async(int batch_size, int num_epochs, int rounds_per_epoch, int window, int probing_interval, int probing_duration, int m_0, struct timeval start_time, int seed, bool use_lock) {

    const unsigned recent_loss_window = 20;
    opt->reset();

    if (seed > 0) {
        m_rng.seed(seed);
    }

    std::vector<XType> x_batches;
    std::vector<YType> y_batches;

    const int nbatch = internal::create_shuffled_batches(x, y, batch_size, m_rng, x_batches, y_batches);

    std::vector<NetworkTopology *> thread_local_networks(num_threads);
    std::vector<MultiClassEntropy *> thread_local_outputs(num_threads);

    // Stuff for saving state, for testing
    std::vector<Optimizer *> saved_thread_local_opts(num_threads);
    std::vector<NetworkTopology *> saved_thread_local_networks(num_threads);

    int current_parallelism = m_0 < 0 ? num_threads / 2 : m_0;
    int latest_epoch = -1;
    double sync_exec_loss;

    ParameterContainer *global_param = net->current_param_container_ptr;
    ParameterContainer *synchronous_param = NULL;

    for (size_t i = 0; i < num_threads; ++i) {
        thread_local_networks[i] = new NetworkTopology(*net);
        thread_local_networks[i]->set_output(
                new MultiClassEntropy(*dynamic_cast<MultiClassEntropy *>(net->get_output())));
    }

    if (rounds_per_epoch < 0) {
        rounds_per_epoch = nbatch;
        // std::cout << "Rounds per epoch set to " << rounds_per_epoch << std::endl;
    }

    std::mutex mtx; // for accessing the shared network object
    std::mutex epoch_time_vector_lock; // for shared times measurements

    // create num_threads x num_epochs matrix for storing thread-local loss sum per epoch
    std::vector<std::vector<Scalar>> local_losses_per_epoch(num_threads);
    std::vector<std::vector<int>> local_rounds_per_epoch(num_threads);
    std::vector<std::vector<long>> local_tau_dist(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        for (int j = 0; j < num_epochs; ++j) {
            local_losses_per_epoch[i].push_back(0);
            local_rounds_per_epoch[i].push_back(0);
        }
        for (int j = 0; j < tau_threshold; ++j) {
            local_tau_dist[i].push_back(0);
        }
    }

    for (int j = 0; j < tau_threshold; ++j) {
        tau_dist_sample.push_back(0);
    }

    std::atomic<int> next_batch(0);
    std::atomic<long> step(0);

    // A separate step counter is used for getting the batch number for synchronous
    // threads. This is so that they cannot "help" the async threads finish epochs;
    // they should be completely independent.
    std::atomic<long> sync_step(0);

    // std::atomic<double> phase_loss;

    int num_iterations = -1;
#ifndef ALL_THREADS_MUST_FINISH
    std::atomic_flag should_stop = ATOMIC_FLAG_INIT;
#endif


    auto sync_point_cbk = []() noexcept {
        // std::cout << "BARRIER REACHED\n";
    };
    std::barrier<typeof(sync_point_cbk)> *sync_point = nullptr;
    std::atomic_int sync_num_finished;

    auto f = [&](int id) {
        int local_iterations = num_iterations;
        
        if (id >= current_parallelism) {
#ifdef SYNC_THREADS
            /* first thread in sync group is responsible for creating barrier */
            if (id == current_parallelism) {
                // sync_num_finished = 0;
                sync_point = new std::barrier(num_threads - current_parallelism, sync_point_cbk);
                std::cout << "BARRIER INIT\n";
            }

            /* If we're not within the asynchronous zone, then we are one of the threads that run SGD
             * synchronously. Until the `should_stop` flag is set from one of the async threads, we will
             * try and just do as many synchronous steps as possible, synchronising with all the other
             * alike threads.
             * Obviously, these sync threads all share the same model, which has to be separate from the
             * model which the async threads use. So, before the threads are all started, a copy of the
             * current global model is made.
             * Since we aren't returning to the main (controlling) thread after each step, one of the
             * sync threads needs to have responsibility for updating the sync model. This will be the
             * thread whose ID == current_parallelism, arbitrarily.
             */
            thread_local_networks[id]->set_pointer(synchronous_param);

            while (!should_stop.test()) {
                long local_step = sync_step.fetch_add(1);
                long batch_index = next_batch.fetch_add(1) % nbatch;
                long epoch_step = local_step % rounds_per_epoch;

                if (local_step >= num_epochs * rounds_per_epoch) {
                    break;
                }

                thread_local_networks[id]->forward(x_batches[batch_index]);
                thread_local_networks[id]->backprop(x_batches[batch_index], y_batches[batch_index]);

                sync_point->arrive_and_wait();
                // std::cout << "sync thread incrementing counter" << std::endl;
                // sync_num_finished++;
                // while (sync_num_finished.load() < num_threads - current_parallelism) { /* busy wait */ }
                // if (id == current_parallelism) {
                //     std::cout << "leader sync thread resetting counter" << std::endl;
                //     sync_num_finished = 0;
                // }
                
                if (id == current_parallelism) {
                    auto net = thread_local_networks[current_parallelism];
                    sync_exec_loss = 0.0;

                    sync_exec_loss += net->get_loss();

                    net->update_cw(thread_local_opts[current_parallelism]);
                    /* std::cout << "[sync] aggregating results from threads " << current_parallelism << " -> " << num_threads - 1 << std::endl; */

                    for (int i = current_parallelism + 1; i < num_threads; i++) {
                        auto subnet = thread_local_networks[i];

                        sync_exec_loss += subnet->get_loss();

                        net->reset();
                        net->aggregate(*subnet);
                        net->normalize_derivatives(num_threads - current_parallelism);
                        net->update_cw(thread_local_opts[current_parallelism]);
                    }

                    sync_exec_loss /= (num_threads - current_parallelism);
                    std::cout << "Aggregate sync loss = " << sync_exec_loss << std::endl;
                }
            }

            /* Here we prevent the deadlock that used to occur when should_stop is set
             * but not all sync threads have arrive_and_wait'ed yet. */
            // std::cout << "sync thread setting counter to final" << std::endl;
            // sync_num_finished = num_threads - current_parallelism;
            auto _ = sync_point->arrive();
#endif
        } else {
            while (true) {
#ifndef ALL_THREADS_MUST_FINISH
                if (should_stop.test()) break; // Stop execution once at least one worker has reached num_iterations
#endif

                if (local_iterations != -1 && local_iterations-- <= 0) {
#ifndef ALL_THREADS_MUST_FINISH
                    should_stop.test_and_set();
                    // std::cout << "[async] should stop, because local_iterations has reached " << local_iterations << "\n";
#endif
                    break;
                }
                
                long local_step = step.fetch_add(1);
                // std::cout << "[" << id << "] step = " << local_step << std::endl;

                if (tauadaptstrat != "NONE" && local_step == tau_sample_stop * num_threads) {
                    // this thread computes the tail distribution
                    compute_tail_dist();
                }

                // reserve next unique batch
                int batch_index = next_batch.fetch_add(1) % nbatch;

                // termination criterion
                if (local_step >= num_epochs * rounds_per_epoch) {
                    // std::cout << "[async] !! Thread has realised it's finished for good!\n";
                    should_stop.test_and_set();
                    break;
                }

                // calculate current epoch
                long epoch = local_step / rounds_per_epoch;
                long epoch_step = local_step % rounds_per_epoch;

                if (use_lock)
                    mtx.lock();

                // copy global to local parameters
                auto *local_param = new ParameterContainer(*global_param);

                if (use_lock)
                    mtx.unlock();

                long t0 = local_param->timestamp;

                // apply new local params to thread
                thread_local_networks[id]->set_pointer(local_param);

                // compute gradient, store in Network object
                thread_local_networks[id]->forward(x_batches[batch_index]);
                thread_local_networks[id]->backprop(x_batches[batch_index], y_batches[batch_index]);
                const Scalar loss = thread_local_networks[id]->get_loss();


                /* std::cerr << id << ": [Epoch " << epoch << "] Loss = " << loss << std::endl; */

                // add loss to thread local epoch loss sum
                local_losses_per_epoch[id][epoch] += loss;
                local_rounds_per_epoch[id][epoch] ++;
                latest_epoch = epoch;

                thread_local_networks[id]->set_pointer(global_param);
                
                delete local_param;

                if (use_lock)
                    mtx.lock();
                long t1 = global_param->timestamp;
                int tau = t1 - t0;
                thread_local_opts[id]->step_scale_factor = get_stepsize_scaling_factor(tau, tauadaptstrat);
                thread_local_networks[id]->update_cw(thread_local_opts[id]); // component-wise update
                if (use_lock)
                    mtx.unlock();

                if (tau < tau_threshold) {
                    local_tau_dist[id][tau] += 1;
                    if (tauadaptstrat != "NONE" && local_step > tau_sample_start * num_threads && local_step < tau_sample_stop * num_threads) {
                        tau_dist_sample[tau] += 1;
                    }
                }

                if (epoch_step == rounds_per_epoch - 1) {
                    struct timeval now;
                    gettimeofday(&now, NULL);

                    epoch_time_vector_lock.lock();
                    time_per_epoch.push_back(now.tv_sec - start_time.tv_sec + (double)(now.tv_usec - start_time.tv_usec)/1000000);
                    epoch_time_vector_lock.unlock();
                }
            }
        }
    };

    std::vector<std::function<void(int id)>> jobs;
    for (int i = 0; i < num_threads; i++) {
        jobs.push_back(f);
    }
    ThreadPool workers(num_threads, jobs);
    // std::cout << "Set up worker pool\n";

    num_iterations = probing_duration;
    workers.wait_for_all();

    long curr_step;
    double avg_loss = 1.0;
    double loss_jitter = 0.0;

    int phase_number = 0;

    while ((curr_step = step.load()) < num_epochs * rounds_per_epoch) {
        // std::cout << "Main loop\n";
        /* Here, all threads are not running.
            * We want to get the loss trend over the previous execution phase.
            * In the previous iteration, there should be `probing_interval` training steps.
            */

        num_iterations = probing_duration;

        // int scaled_window = window * std::min(avg_loss, 2.0); // Don't scale the window to any more than 2x original
        const int scaled_window = window; // Don't scale the window at all
        int best_m = -1;
        const unsigned m_last = current_parallelism;

        int window_skew;
        if (loss_grads.empty()) {
            window_skew = 0;
        } else { /* Calculate window skew */
            // If our loss is getting better, then we can try pushing the window
            // up, to try some more aggressive parallelism.
            // If it's getting worse, then maybe the async induced noise due to staleness is getting
            // too much, so try skewing the window down.
            
            /* Take last n values from m_values and get trend */
            double m_trend = 0.0;
            if (m_exec_values.size() >= 2) { /* We need at least two values to get differences */
                // std::cout << "Doing trend calculation\n";
                int last = m_exec_values.size() - 1;
                int first = last - 3;
                if (first < 0) first = 0;
                // std::cout << "\tm_trend window: " << first << " -> " << last << std::endl;

                for (size_t i = first; i < last; i++) {
                    // std::cout << "\tAdding diff to m_trend: " << m_exec_values[i + 1] << " - " << m_exec_values[i] << " = " << m_exec_values[i+1] - m_exec_values[i] << std::endl;
                    m_trend += (double)(m_exec_values[i + 1] - m_exec_values[i]);
                }
                // std::cout << "m_trend = " << m_trend << " / " << (last-first) + 1 << std::endl;
                m_trend /= (last - first) + 1;
            }

            /* 
             * Intuitively, it makes sense that if the parallelism trend is heading downwards, we would like to
             * carry on in that direction, to account for any small spikes that might otherwise change our mind.
             * This is kind of like momentum.
             *
             * If the recent model performance has been varying a lot (i.e. has high jitter), then we may want to
             * nudge the window down to try and aim towards lower parallelisms, while spending less time probing
             * in higher than desired levels.
             *
             * If the loss has been consistently decreasing over the previous window, then even if there's a short
             * moment of increasing loss, it may still make sense to raise the window up. If the loss has been
             * consistently increasing, then clearly our current strategy is failing, probably too agressive, so
             * we can try to reduce the parallelism.
             */
            window_skew = loss_grads.back() * scalar_loss_grad + loss_jitter * scalar_loss_jitter + m_trend * scalar_m_trend;
            // std::cout << "Loss grad = " << loss_grads.back() << ", Loss jitter = " << loss_jitter << ", M trend = " << m_trend << std::endl;
            // std::cout << "Skewing window by " << window_skew << std::endl;
        }

#ifdef STANDARD_WINDOW
        int window_top = m_last + scaled_window/2;
        int window_btm = window_top - scaled_window + 1;
        int window_step = 1;
#endif

#ifdef SHIFT_WINDOW
        // Contruct top and bottom of window. If the window would ordinarily go off the "screen",
        // then shift it down so that it touches the top.
        int window_top = m_last + scaled_window/2 + window_skew;
        if (window_top >= num_threads) 
            window_top = num_threads - 1;
        int window_btm = window_top - scaled_window;
        if (window_btm < 1) {
            window_btm = 1;
            window_top = window_btm + scaled_window;
        }
        int window_step = 1;
#endif
        
#ifdef PROBE_ENTIRE_SPACE
        int window_step = 8;
        int window_btm = 1;
        int window_top = num_threads - 1;
#endif

#ifdef EXTEND_WINDOW
        int window_top = m_last + scaled_window/2;
        int window_btm = window_top - (scaled_window - 1);

        if (window_skew < 0) {
            window_btm -= window_skew;
        } else {
            window_top += window_skew;
        }

        window_btm = window_btm < 1 ? 1 : window_btm;
        window_top = window_top >= num_threads ? num_threads-1 : window_top;
        int window_step = 1;
#endif

#ifdef PROBE_WHOLE
        int window_top = num_threads;
        int window_btm = 1;
        int window_step = 8; /* could vary this */
#endif

        m_probe_starts.push_back(m_values.size());

        double best_probe_delta_loss = std::numeric_limits<double>::infinity();
        double prev_loss = 0; // The previous loss value, before probing

        /* If the networks have previous loss values then use them, otherwise this is the first iteration */
        if (thread_local_networks[0]->get_loss() >= 0.0) {
            // TODO: Should this be the most recent epoch loss instead?
            for (int i = 0; i < current_parallelism; i++) {
                prev_loss += thread_local_networks[i]->get_loss();
            }
            prev_loss /= current_parallelism;
        } else {
            prev_loss = -1; // Flag saying that it's unset
        }

        ParameterContainer *param_save = new ParameterContainer(*global_param);
        copy_opts_vec(thread_local_opts, saved_thread_local_opts);
        copy_nets_vec(thread_local_networks, saved_thread_local_networks);

        std::cout << "# starting probing\n";
        // Run a probing phase for each m in the m-window
        for (int m = window_btm; m <= window_top; m += window_step) {
            // Make fresh state for fair comparison
            global_param = new ParameterContainer(*param_save);
            copy_opts_vec(saved_thread_local_opts, thread_local_opts);
            copy_nets_vec(saved_thread_local_networks, thread_local_networks);

            // std::cout << "Starting probing loop at " << m << std::endl;
            current_parallelism = m;
            
            if (current_parallelism >= num_threads) break;
            if (current_parallelism < 1) continue;
            
            struct timeval probe_start, probe_end;
            gettimeofday(&probe_start, NULL);

            m_values.push_back(current_parallelism);
            m_times.push_back((double)(probe_start.tv_sec - start_time.tv_sec) + (double)(probe_start.tv_usec - start_time.tv_usec)/1000000); 

#ifndef ALL_THREADS_MUST_FINISH
            should_stop.clear();
#endif

#ifdef SYNC_THREADS
            synchronous_param = new ParameterContainer(*global_param);
#endif

            workers.start_all();
            workers.wait_for_all();

            gettimeofday(&probe_end, NULL);
            double probe_elapsed = (double)(probe_end.tv_usec - probe_start.tv_usec)/1000000;

            double loss = 0;
            for (int i = 0; i < current_parallelism; i++) {
                if (std::isnan(thread_local_networks[i]->get_loss())) {
                    std::cout << "WARNING: network " << i << " had loss of nan!\n";
                }
                loss += thread_local_networks[i]->get_loss();
            }
            loss /= current_parallelism; // average los of each model
            
            if (prev_loss < 0) prev_loss = loss;
            double loss_diff = loss - prev_loss;

            std::cout << phase_number << ", " << current_parallelism << ", " << loss << std::endl;

            prev_loss = loss;

            /* update_loss_grad(loss, start_time); */

            if (loss_diff < best_probe_delta_loss) {
                best_probe_delta_loss = loss_diff;
                best_m = current_parallelism;
            }
        }

        m_probe_ends.push_back(m_values.size());

        // After probing, run normal async execution for a while
        current_parallelism = best_m;

        synchronous_param = new ParameterContainer(*global_param);

        std::cout << "# <execution> current_parallelism == " << current_parallelism << std::endl;
        m_exec_values.push_back(current_parallelism);

        struct timeval now;
        gettimeofday(&now, NULL);

        m_values.push_back(current_parallelism);
        m_times.push_back((double)(now.tv_sec - start_time.tv_sec) + (double)(now.tv_usec - start_time.tv_usec)/1000000); 

        num_iterations = probing_interval;
#ifndef ALL_THREADS_MUST_FINISH
        should_stop.clear();
#endif
        workers.start_all();
        workers.wait_for_all();

        double avg_loss = 0;
        for (int th = 0; th < current_parallelism; th++) {
            double l = thread_local_networks[th]->get_loss();
            avg_loss += l;
        }
        avg_loss /= current_parallelism;

        update_loss_grad(avg_loss, start_time);

        /* First check if we want to base the next execution phase on the current
         * sync or async mode status */

#ifdef SYNC_THREADS
        std::cout << "sync loss = " << sync_exec_loss << std::endl;
        if (sync_exec_loss < avg_loss) {
            std::cout << "Got better loss in synchronous execution, so replacing global model with it!\n";
            std::cout << " - " << sync_exec_loss << " < " << avg_loss << std::endl;
            global_param = new ParameterContainer(*synchronous_param);
        }
#endif

        /* Now we analyse the performance of recent async mode to calculate:
         *  - Jitter of previous window
         *  - Average trend of parallelism in previous window
         */

        std::vector<double> all_epoch_avgs(num_epochs);
        std::vector<int> all_epoch_contributors(num_epochs);

        int epoch_win_first = latest_epoch - 8;
        if (epoch_win_first < 0) epoch_win_first = 0;
        // std::cout << "Epoch window = " << epoch_win_first << " -> " << latest_epoch << std::endl;

        for (int i = 0; i < num_threads; i++) {
            for (int j = epoch_win_first; j <= latest_epoch; j++) {
                double l = local_losses_per_epoch[i][j];
                int rounds_done = local_rounds_per_epoch[i][j];
                if (l == 0) continue;
                all_epoch_avgs[j] += l / rounds_done;
                all_epoch_contributors[j]++;
            }
        }

        /* We want to calculate the standard deviation of pairwise differences for average losses for the epochs in the window */
        double sd;

        { /* Calculate sd of diffs */
            double prev_avg = -1.0;
            std::vector<double> window_diffs;
            double mean_of_diffs;

            /* First we make a new vector based on the differences between each two adjacent elements in the previously
            * calculated vector of all (up until latest) epoch losses */
            for (int e = epoch_win_first; e <= latest_epoch; e++) {
                double this_avg = all_epoch_avgs[e] / all_epoch_contributors[e];
                if (prev_avg >= 0.0) {
                    double diff = this_avg - prev_avg;
                    window_diffs.push_back(diff);
                    mean_of_diffs += diff;
                }
                prev_avg = this_avg;
            }
            mean_of_diffs /= window_diffs.size();
            sd = 0.0;
            for (double diff : window_diffs) {
                double x = std::abs(diff - mean_of_diffs);
                sd += x * x;
            }
            sd = std::sqrt(sd / window_diffs.size());
        }

        // std::cout << "SD calculated as " << sd << std::endl;
        
        loss_jitter = sd;
        phase_number++;
    }


    workers.stop();

    for (int k = 0; k < num_epochs; k++) {
        loss = 0;
        for (int i = 0; i < num_threads; i++) {
            loss += local_losses_per_epoch[i][k];
        }
        loss /= rounds_per_epoch;
        loss_per_epoch.push_back(loss);
    }
    for (int tau = 0; tau < tau_threshold; tau++) {
        int tau_count = 0;
        for (int i = 0; i < num_threads; i++) {
            tau_count += local_tau_dist[i][tau];
        }
        tau_dist.push_back(tau_count);
    }
}
