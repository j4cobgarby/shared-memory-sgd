#include "NetworkExecutor.h"
#include "NetworkTopology.h"
#include "Optimizer.h"
#include "ParameterContainer.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <chrono>

using namespace std::literals;

#define PROBE_WHOLE

void copy_opts_vec(std::vector<MiniDNN::Optimizer *> &from, std::vector<MiniDNN::Optimizer *> &to) {
    assert(from.size() == to.size());

    for (int i = 0; i < from.size(); i++) {
        delete to.at(i);
        to.at(i) = from.at(i)->clone();
    }
}

void copy_nets_vec(std::vector<MiniDNN::NetworkTopology *> &from,
                   std::vector<MiniDNN::NetworkTopology *> &to) {
    assert(from.size() == to.size());

    for (int i = 0; i < from.size(); i++) {
        delete to.at(i);
        to.at(i) = new MiniDNN::NetworkTopology(*from.at(i));
    }
}

void MiniDNN::NetworkExecutor::run_elastic_async(int batch_size, int num_epochs,
                                                 int rounds_per_epoch, int window,
                                                 int probing_interval, int probing_duration,
                                                 int m_0, struct timeval start_time, int seed,
                                                 bool use_lock) {
    const unsigned recent_loss_window = 20;
    opt->reset();

    if (seed > 0) {
        m_rng.seed(seed);
    }

    std::vector<XType> x_batches;
    std::vector<YType> y_batches;

    const int nbatch =
        internal::create_shuffled_batches(x, y, batch_size, m_rng, x_batches, y_batches);

    std::vector<NetworkTopology *> thread_local_networks(num_threads);
    std::vector<MultiClassEntropy *> thread_local_outputs(num_threads);

    // Stuff for saving state, for testing
    #ifdef PROBING_TEST
    std::vector<Optimizer *> saved_thread_local_opts(num_threads, nullptr);
    std::vector<NetworkTopology *> saved_thread_local_networks(num_threads, nullptr);
    #endif

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

    std::mutex mtx;                    // for accessing the shared network object
    std::mutex epoch_time_vector_lock; // for shared times measurements

    // create num_threads x num_epochs matrix for storing thread-local loss sum
    // per epoch
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
    long phase_firststep = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> phase_starttime;

    // std::atomic<double> phase_loss;

    int num_iterations = -1;
    std::atomic_flag should_stop = ATOMIC_FLAG_INIT;

    auto f = [&](int id) {
        if (id >= current_parallelism) {
            return;
        } else {
            while (true) {
                if (should_stop.test())
                    break; // Stop execution once at least one worker has reached
                           // num_iterations

                long local_step = step.fetch_add(1);

                if (std::chrono::high_resolution_clock::now() - phase_starttime > 1s) {
                    break;
                }

                if (tauadaptstrat != "NONE" && local_step == tau_sample_stop * num_threads) {
                    // this thread computes the tail distribution
                    compute_tail_dist();
                }

                // reserve next unique batch
                int batch_index = next_batch.fetch_add(1) % nbatch;

                // #ifndef PROBING_TEST // For the probing test, we want to make sure we don't finish
                //                      // until we try all probes
                // if (local_step >= num_epochs * rounds_per_epoch) {
                //   should_stop.test_and_set();
                //   break;
                // }
                // #endif

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

                /* std::cerr << id << ": [Epoch " << epoch << "] Loss = " << loss <<
                 * std::endl; */

                // add loss to thread local epoch loss sum
                local_losses_per_epoch[id][epoch] += loss;
                local_rounds_per_epoch[id][epoch]++;
                latest_epoch = epoch;

                thread_local_networks[id]->set_pointer(global_param);

                delete local_param;

                if (use_lock)
                    mtx.lock();
                long t1 = global_param->timestamp;
                int tau = t1 - t0;
                thread_local_opts[id]->step_scale_factor =
                    get_stepsize_scaling_factor(tau, tauadaptstrat);
                thread_local_networks[id]->update_cw(
                    thread_local_opts[id]); // component-wise update
                if (use_lock)
                    mtx.unlock();

                if (tau < tau_threshold) {
                    local_tau_dist[id][tau] += 1;
                    if (tauadaptstrat != "NONE" && local_step > tau_sample_start * num_threads &&
                        local_step < tau_sample_stop * num_threads) {
                        tau_dist_sample[tau] += 1;
                    }
                }

                if (epoch_step == rounds_per_epoch - 1) {
                    struct timeval now;
                    gettimeofday(&now, NULL);

                    epoch_time_vector_lock.lock();
                    time_per_epoch.push_back(now.tv_sec - start_time.tv_sec +
                                             (double)(now.tv_usec - start_time.tv_usec) / 1000000);
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

    std::cout << "# Phase num, Parallelism, Loss, Time taken (s), repeat num\n";

    // while ((curr_step = step.load()) < num_epochs * rounds_per_epoch) {
    while (phase_number <= 20) {
        /* Here, all threads are not running.
         * We want to get the loss trend over the previous execution phase.
         * In the previous iteration, there should be `probing_interval` training
         * steps.
         */

        num_iterations = probing_duration;

        // int scaled_window = window * std::min(avg_loss, 2.0); // Don't scale the
        // window to any more than 2x original
        const int scaled_window = window; // Don't scale the window at all
        int best_m = -1;
        const unsigned m_last = current_parallelism;

        // int window_skew;
        if (loss_grads.empty()) {
            // window_skew = 0;
        } else { /* Calculate window skew */
            // If our loss is getting better, then we can try pushing the window
            // up, to try some more aggressive parallelism.
            // If it's getting worse, then maybe the async induced noise due to
            // staleness is getting too much, so try skewing the window down.

            /* Take last n values from m_values and get trend */
            double m_trend = 0.0;
            if (m_exec_values.size() >= 2) { /* We need at least two values to get differences */
                // std::cout << "Doing trend calculation\n";
                int last = m_exec_values.size() - 1;
                int first = last - 3;
                if (first < 0)
                    first = 0;
                // std::cout << "\tm_trend window: " << first << " -> " << last <<
                // std::endl;

                for (size_t i = first; i < last; i++) {
                    // std::cout << "\tAdding diff to m_trend: " << m_exec_values[i + 1]
                    // << " - " << m_exec_values[i] << " = " << m_exec_values[i+1] -
                    // m_exec_values[i] << std::endl;
                    m_trend += (double)(m_exec_values[i + 1] - m_exec_values[i]);
                }
                // std::cout << "m_trend = " << m_trend << " / " << (last-first) + 1 <<
                // std::endl;
                m_trend /= (last - first) + 1;
            }
        }

#ifdef PROBE_WHOLE
        int window_top = num_threads;
        int window_btm = 1;
        int window_step = 8; /* could vary this */
#endif

        m_probe_starts.push_back(m_values.size());

        double best_probe_delta_loss = std::numeric_limits<double>::infinity();
        double prev_loss = 0; // The previous loss value, before probing

        /* If the networks have previous loss values then use them, otherwise this
         * is the first iteration */
        if (thread_local_networks[0]->get_loss() >= 0.0) {
            for (int i = 0; i < current_parallelism; i++) {
                prev_loss += thread_local_networks[i]->get_loss();
            }
            prev_loss /= current_parallelism;
        } else {
            prev_loss = -1; // Flag saying that it's unset
        }

        std::cout << "# starting probing\n";

        std::cout << "# probing from " << window_btm << " to max " << window_top << ", step=" << window_step << std::endl;

        for (int m = window_btm; m <= window_top; m += window_step) {
            // std::cout << "Starting probing loop at " << m << std::endl;
            current_parallelism = m;
            phase_firststep = step;

            if (current_parallelism >= num_threads)
                break;
            if (current_parallelism < 1)
                continue;

            struct timeval probe_start, probe_end;
            gettimeofday(&probe_start, NULL);

            m_values.push_back(current_parallelism);
            m_times.push_back((double)(probe_start.tv_sec - start_time.tv_sec) +
                              (double)(probe_start.tv_usec - start_time.tv_usec) / 1000000);

            should_stop.clear();

            auto work_start = std::chrono::high_resolution_clock::now();
            phase_starttime = work_start;

            std::cout << "start_for_all...\n";
            workers.start_all();
            workers.wait_for_all();
            std::cout << "wait_for_all done\n";

            auto work_end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> dur = work_end - work_start;
            double work_dur = dur.count();

            gettimeofday(&probe_end, NULL);

            std::cout << "calculating loss...\n";
            double loss = 0;
            int loss_contributors = 0;
            for (int i = 0; i < current_parallelism; i++) {
                if (!std::isnan(thread_local_networks[i]->get_loss())) {
                    loss_contributors++;
                    // std::cout << "# WARNING: network " << i << " had loss of nan!\n";
                }
                loss += thread_local_networks[i]->get_loss();
            }
            loss /= loss_contributors; // average los of each model
            std::cout << "calculating loss complete.\n";

            if (prev_loss < 0)
                prev_loss = loss;
            double loss_diff = loss - prev_loss;

            std::cout << phase_number << ", " << current_parallelism << ", " << loss << ", " << work_dur << ", " << std::endl;

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
