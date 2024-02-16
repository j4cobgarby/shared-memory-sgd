#include "NetworkExecutor.h"

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

    int current_parallelism = m_0 < 0 ? num_threads / 2 : m_0;

    ParameterContainer *global_param = net->current_param_container_ptr;

    for (size_t i = 0; i < num_threads; ++i) {
        thread_local_networks[i] = new NetworkTopology(*net);
        thread_local_networks[i]->set_output(
                new MultiClassEntropy(*dynamic_cast<MultiClassEntropy *>(net->get_output())));
    }

    if (rounds_per_epoch < 0) {
        rounds_per_epoch = nbatch;
        std::cout << "Rounds per epoch set to " << rounds_per_epoch << std::endl;
    }

    std::mutex mtx; // for accessing the shared network object
    std::mutex epoch_time_vector_lock; // for shared times measurements

    // create num_threads x num_epochs matrix for storing thread-local loss sum per epoch
    std::vector<std::vector<Scalar>> local_losses_per_epoch(num_threads);
    std::vector<std::vector<long>> local_tau_dist(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        for (int j = 0; j < num_epochs; ++j) {
            local_losses_per_epoch[i].push_back(0);
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

    // std::atomic<double> phase_loss;

    int num_iterations = -1;

    auto f = [&](int id) {
        // std::cout << "Starting learning thread function proper" << std::endl;
        int local_iterations = num_iterations;
        
        if (id >= current_parallelism) {
            return;
        }

        int iters = 0;
        
        while (true) {
            iters++;
            // std::cout << id << ") iteration counter = " << local_iterations << std::endl;
            if (local_iterations != -1 && local_iterations-- <= 0) break;
            
            long local_step = step.fetch_add(1);
            //std::cout << "thread " << id << " step " << local_step << std::endl;

            if (tauadaptstrat != "NONE" && local_step == tau_sample_stop * num_threads) {
                // this thread computes the tail distribution
                compute_tail_dist();
            }

            // reserve next unique batch
            int batch_index = next_batch.fetch_add(1) % nbatch;

            // termination criterion
            if (local_step >= num_epochs * rounds_per_epoch) {
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


            //std::cerr << id << ": [Epoch " << epoch << "] Loss = " << loss << std::endl;

            // add loss to thread local epoch loss sum
            local_losses_per_epoch[id][epoch] += loss;

            thread_local_networks[id]->set_pointer(global_param);
            
            delete local_param;

            if (use_lock)
                mtx.lock();

            long t1 = global_param->timestamp;

            int tau = t1 - t0;

            thread_local_opts[id]->step_scale_factor = get_stepsize_scaling_factor(tau, tauadaptstrat);

            //thread_local_networks[id]->update(opt); // AlignedMapVec update
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
                time_per_epoch.push_back(now.tv_sec - start_time.tv_sec);
                epoch_time_vector_lock.unlock();
            }
        }
    };

    std::vector<std::function<void(int id)>> jobs;

    for (int i = 0; i < num_threads; i++) {
        jobs.push_back(f);
    }

    ThreadPool workers(num_threads, jobs);

    std::cout << "Set up worker pool\n";

    num_iterations = probing_duration;

    // Wait for all workers to be ready to go
    workers.wait_for_all();

    long curr_step;
    double avg_loss = 1.0;

    while ((curr_step = step.load()) < num_epochs * rounds_per_epoch) {
        /* Here, all threads are not running.
            * We want to get the loss trend over the previous execution phase.
            * In the previous iteration, there should be `probing_interval` training steps.
            */

        num_iterations = probing_duration;
        double best_loss = std::numeric_limits<double>::infinity();

        // int scaled_window = window * std::min(avg_loss, 2.0); // Don't scale the window to any more than 2x original
        int scaled_window = window; // Don't scale the window at all
        int best_m = -1;
        unsigned m_last = current_parallelism;

        int window_skew;
        if (loss_grads.empty()) {
            window_skew = 0;
        } else {
            // If our loss is getting better, then we can try pushing the window
            // up, to try some more aggressive parallelism.
            // If it's getting worse, then maybe the async induced noise is getting
            // too much, so try skewing the window down.
            window_skew = loss_grads.back() * -20;
            std::cout << "Skewing window by " << window_skew << std::endl;
        }

        // Contruct top and bottom of window. If the window would ordinarily go off the "screen",
        // then shift it down so that it touches the top.
        // int window_top = m_last + scaled_window/2 + window_skew;
        // if (window_top >= num_threads) window_top = num_threads - 1;
        // int window_btm = window_top - scaled_window;
        // if (window_btm < 1) {
        //     window_btm = 1;
        //     window_top = window_btm + scaled_window;
        // }
        
        /* int window_top = m_last + scaled_window/2; */
        /* int window_btm = window_top - (scaled_window - 1); */
        int window_step = 8;
        int window_btm = 1;
        int window_top = num_threads - 1;

        /* if (window_skew < 0) { */
        /*     window_btm -= window_skew; */
        /* } else { */
        /*     window_top += window_skew; */
        /* } */
        /**/
        /* window_btm = window_btm < 1 ? 1 : window_btm; */
        /* window_top = window_top >= num_threads ? num_threads-1 : window_top; */

        std::cout << "m_last = " << m_last << "\n";
        std::cout << "Window = ["<<window_btm << ", " << window_top << "]\n";

        // Run a probing phase for each m in the m-window
        for (int m = window_btm; m <= window_top; m += window_step) {
            current_parallelism = m;
            
            if (current_parallelism >= num_threads) break;
            if (current_parallelism < 1) continue;
            
            struct timeval probe_start, probe_end;
            gettimeofday(&probe_start, NULL);

            m_values.push_back(current_parallelism);
            m_times.push_back((double)(probe_start.tv_sec - start_time.tv_sec) + (double)(probe_start.tv_usec - start_time.tv_usec)/1000000); 
            std::cout << "current_parallelism == " << current_parallelism << std::endl;

            workers.start_all();
            workers.wait_for_all();

            gettimeofday(&probe_end, NULL);
            double probe_elapsed = (double)(probe_end.tv_usec - probe_start.tv_usec)/1000000;

            double loss = 0;
            for (int i = 0; i < current_parallelism; i++) {
                loss += thread_local_networks[i]->get_loss();
            }
            loss /= current_parallelism; // average los of each model
            // loss *= probe_elapsed; // shorter elapsed time -> lower `loss` -> better "score"
            
            /* update_loss_grad(loss, start_time); */
            /* prev_loss = loss; */

            if (loss < best_loss) {
                best_loss = loss;
                best_m = current_parallelism;
            }
        }

        // After probing, run normal async execution for a while
        current_parallelism = best_m;

        std::cout << "<execution> current_parallelism == " << current_parallelism << std::endl;

        struct timeval now;
        gettimeofday(&now, NULL);

        m_values.push_back(current_parallelism);
        m_times.push_back((double)(now.tv_sec - start_time.tv_sec) + (double)(now.tv_usec - start_time.tv_usec)/1000000); 

        num_iterations = probing_interval;
        workers.start_all();
        workers.wait_for_all();

        avg_loss = 0;
        for (int th = 0; th < current_parallelism; th++) {
            avg_loss += thread_local_networks.at(th)->get_loss();
        }
        avg_loss /= num_threads;

        update_loss_grad(avg_loss, start_time);
    }


    workers.stop();

    std::cout << "Training finished." << std::endl;

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

    std::cout << "Returning" << std::endl;
}