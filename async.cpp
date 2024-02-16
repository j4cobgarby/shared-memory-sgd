#include "NetworkExecutor.h"

void MiniDNN::NetworkExecutor::run_parallel_async(int batch_size, int num_epochs, int rounds_per_epoch, struct timeval start_time, int seed, bool use_lock) {

    opt->reset();

    // Create shuffled mini-batches
    if (seed > 0) {
        m_rng.seed(seed);
    }

    std::vector<XType> x_batches;
    std::vector<YType> y_batches;
    //std::vector<MultiClassEntropy*> output_layers;
    const int nbatch = internal::create_shuffled_batches(x, y, batch_size, m_rng,
                                                            x_batches, y_batches);

    std::vector<NetworkTopology *> thread_local_networks(num_threads);
    std::vector<MultiClassEntropy *> thread_local_outputs(num_threads);

    ParameterContainer *global_param = net->current_param_container_ptr;

    for (size_t i = 0; i < num_threads; ++i) {
        thread_local_networks[i] = new NetworkTopology(*net);
        thread_local_networks[i]->set_output(
                new MultiClassEntropy(*dynamic_cast<MultiClassEntropy *>(net->get_output())));
    }

    if (rounds_per_epoch < 0) {
        rounds_per_epoch = nbatch;
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


    auto f = [&](int id) {
        while (true) {
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

    for (int i = 0; i < num_threads; ++i) {
        jobs.push_back(f);
    }

    ThreadPool workers(num_threads, jobs);

    //std::cout << "wait for workers" << std::endl;
    workers.wait_for_all();
    //std::cout << "start training" << std::endl;
    workers.start_all();
    //std::cout << "wait for workers" << std::endl;
    workers.wait_for_all();

    //std::cout << "stop training" << std::endl;
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

