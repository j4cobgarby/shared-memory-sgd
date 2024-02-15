#include "NetworkExecutor.h"

void MiniDNN::NetworkExecutor::run_parallel_leashed(int batch_size, int num_epochs, int rounds_per_epoch, int CAS_backoff, bool check_concurrent_updates, int seed) {

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

    ParameterContainer *init_param = net->current_param_container_ptr;
    ParameterContainer **param_pointer = &init_param;

    if (check_concurrent_updates)
        net->param_pointer = param_pointer;

    for (size_t i = 0; i < num_threads; ++i) {
        thread_local_networks[i] = new NetworkTopology(*net);
        thread_local_networks[i]->set_output(
                new MultiClassEntropy(*dynamic_cast<MultiClassEntropy *>(net->get_output())));
    }

    if (rounds_per_epoch < 0) {
        rounds_per_epoch = nbatch;
    }

    // create num_threads x num_epochs matrix for storing thread-local loss sum per epoch
    std::vector<std::vector<Scalar>> local_losses_per_epoch(num_threads);
    std::vector<std::vector<long>> local_tau_dist(num_threads);
    std::vector<std::vector<long>> local_num_tries_dist(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        for (int j = 0; j < num_epochs; ++j) {
            local_losses_per_epoch[i].push_back(0);
        }
        for (int j = 0; j < tau_threshold; ++j) {
            local_tau_dist[i].push_back(0);
        }
        for (int j = 0; j < CAS_backoff; ++j) {
            local_num_tries_dist[i].push_back(0);
        }
    }
    for (int j = 0; j < tau_threshold; ++j) {
        tau_dist_sample.push_back(0);
    }

    std::atomic<int> failed_cas_counter(0);
    std::atomic<int> next_batch(0);
    std::atomic<long> step(0);

    bool end_flag = false;

    auto f = [&](int id) {
        ParameterContainer *latest_pointer;
        while (true) {

            if (end_flag) {
                break;
            }

            long local_step = step.load();

            // reserve next unique batch
            int batch_index = next_batch.fetch_add(1) % nbatch;

            // fetch latest parameters
            bool fetch_done = false;
            while (!fetch_done) {
                latest_pointer = *(param_pointer);
                latest_pointer->active_readers.fetch_add(1);
                if (!latest_pointer->stale_flag.load()) {
                    break;
                } else {
                    latest_pointer->active_readers.fetch_add(-1);
                    // safe delete
                    if (latest_pointer->stale_flag.load()
                        && latest_pointer->active_readers.load() == 0
                        && BOOL_CAS(&(latest_pointer->deleted), false, true)) {
                        latest_pointer->delete_theta();
                    }
                }
            }

            long t0 = latest_pointer->timestamp;

            // termination criterion
            if (t0 >= num_epochs * rounds_per_epoch) {
                end_flag = true;
                break;
            }

            thread_local_networks[id]->set_pointer(latest_pointer);

            struct timeval grad_comp_start, grad_comp_end;
            gettimeofday(&grad_comp_start, NULL);

            // compute gradient, store in Network object
            thread_local_networks[id]->forward(x_batches[batch_index]);
            thread_local_networks[id]->backprop(x_batches[batch_index], y_batches[batch_index]);

            gettimeofday(&grad_comp_end, NULL);
            time_t duration;
            duration = (grad_comp_end.tv_sec - grad_comp_start.tv_sec) * 1000000;
            duration += (grad_comp_end.tv_usec - grad_comp_start.tv_usec);

            //std::cout << duration << std::endl;

            latest_pointer->active_readers.fetch_add(-1);
            // safe delete
            if (latest_pointer->stale_flag.load()
                && latest_pointer->active_readers.load() == 0
                && BOOL_CAS(&(latest_pointer->deleted), false, true)) {
                latest_pointer->delete_theta();
            }

            const Scalar loss = thread_local_networks[id]->get_loss();

            int num_tries = 0;

            while (true) {

                if (end_flag) {
                    break;
                }

                // fetch latest parameters
                bool fetch_done = false;
                while (!fetch_done) {
                    latest_pointer = *(param_pointer);
                    latest_pointer->active_readers.fetch_add(1);

                    if (!latest_pointer->stale_flag.load()) {
                        break;
                    } else {
                        latest_pointer->active_readers.fetch_add(-1);
                        // safe delete
                        if (latest_pointer->stale_flag.load()
                            && latest_pointer->active_readers.load() == 0
                            && BOOL_CAS(&(latest_pointer->deleted), false, true)) {
                            latest_pointer->delete_theta();
                        }
                    }
                }


                //std::cout << id << ": 2" << 1 << std::endl;

                long t1 = latest_pointer->timestamp;

                //std::cout << "thread " << id << ": " << "1" << std::endl;

                // termination criterion
                if (t1 >= num_epochs * rounds_per_epoch) {
                    end_flag = true;
                    break;
                }

                int tau = t1 - t0;

                // copy latest parameters to local memory
                auto local_params = new ParameterContainer(*latest_pointer);

                latest_pointer->active_readers.fetch_add(-1);
                // safe delete
                if (latest_pointer->stale_flag.load()
                    && latest_pointer->active_readers.load() == 0
                    && BOOL_CAS(&(latest_pointer->deleted), false, true)) {
                    latest_pointer->delete_theta();
                }

                thread_local_networks[id]->set_pointer(local_params);

                if (check_concurrent_updates)
                    thread_local_networks[id]->latest_pointer = latest_pointer;

                struct timeval update_start, update_end;
                gettimeofday(&update_start, NULL);

                float scale_factor = get_stepsize_scaling_factor(tau, tauadaptstrat);
                thread_local_opts[id]->step_scale_factor = scale_factor;

                // apply gradient locally
                //thread_local_networks[id]->update(opt); // AlignedMapVec update
                thread_local_networks[id]->update_cw(thread_local_opts[id]); // component-wise update

                gettimeofday(&update_end, NULL);
                time_t duration;
                duration = (update_end.tv_sec - update_start.tv_sec) * 1000000;
                duration += (update_end.tv_usec - update_start.tv_usec);

                //std::cout << duration << std::endl;

                // set local parameters to global if unchanged
                if (!check_concurrent_updates || *param_pointer == latest_pointer) {
                    if (BOOL_CAS(param_pointer, latest_pointer, local_params)) {

                        latest_pointer->stale_flag.store(true);

                        // safe delete
                        if (latest_pointer->stale_flag.load()
                            && latest_pointer->active_readers.load() == 0
                            && BOOL_CAS(&(latest_pointer->deleted), false, true)) {
                            latest_pointer->delete_theta();
                        }

                        // termination criterion
                        if (t1 + 1 >= num_epochs * rounds_per_epoch) {
                            end_flag = true;
                            break;
                        }

                        // calculate current epoch
                        long epoch = local_step / rounds_per_epoch;

                        // add loss to thread local epoch loss sum
                        local_losses_per_epoch[id][epoch] += loss;

                        local_step = step.fetch_add(1);
                        long epoch_step = local_step % rounds_per_epoch;

                        if (tauadaptstrat != "NONE" && local_step == tau_sample_stop * num_threads) {
                            // this thread computes the tail distribution
                            compute_tail_dist();
                        }

                        if (epoch_step == rounds_per_epoch - 1) {
                            struct timeval now;
                            gettimeofday(&now, NULL);

                            if (!time_per_epoch.empty()) {
                                if (time_per_epoch.back() > now.tv_sec)
                                    std::cout << "thread: " << id << ": " << "WARNING: inconsistent epoch time values" << std::endl;
                            }
                            time_per_epoch.push_back(now.tv_sec);
                        }

                        if (tau < tau_threshold) {
                            local_tau_dist[id][tau] += 1;
                            if (tauadaptstrat != "NONE" && local_step > tau_sample_start * num_threads && local_step < tau_sample_stop * num_threads) {
                                tau_dist_sample[tau] += 1;
                            }
                        }

                        if (num_tries < CAS_backoff) {
                            local_num_tries_dist[id][num_tries] += 1;
                        } else {
                            //std::cout << "thread: " << id << ": " << "t" << id << " num_tries: " << num_tries << std::endl;
                        }

                        break;
                    }

                    failed_cas_counter.fetch_add(1);
                }

                delete local_params;

                num_tries++;
                if (num_tries > CAS_backoff && CAS_backoff != 200)
                    break;
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
    //std::cout << "wait for all" << std::endl;
    workers.wait_for_all();
    //std::cout << "stop" << std::endl;

    //std::cout << "stop training" << std::endl;
    workers.stop();
    //std::cout << "comp failed cas" << std::endl;

    //std::cout << "steps: " << step.load() << std::endl;

    failed_cas = failed_cas_counter.load();

    //std::cout << "comp statistics" << std::endl;

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
    for (int num_tries = 0; num_tries < CAS_backoff; num_tries++) {
        int num_tries_count = 0;
        for (int i = 0; i < num_threads; i++) {
            num_tries_count += local_num_tries_dist[i][num_tries];
        }
        num_tries_dist.push_back(num_tries_count);
    }

    //std::cout << loss_per_epoch.size() << std::endl;
    //std::cout << tau_dist.size() << std::endl;
    //std::cout << num_tries_dist.size() << std::endl;

    //std::cout << "end executor" << std::endl;
}

