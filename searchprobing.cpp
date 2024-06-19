#include "NetworkExecutor.h"
#include "ParameterContainer.h"
#include <sys/select.h>

void MiniDNN::NetworkExecutor::run_search_probing(int bs, int ep_n, int ep_r, int m0, struct timeval start_time, int seed) {
    this->opt->reset();

    if (seed > 0) {
        this->m_rng.seed(seed);
    }

    std::vector<XType> x_batches;
    std::vector<YType> y_batches;

    const int nbatch = internal::create_shuffled_batches(this->x, this->y, bs, m_rng, x_batches, y_batches);

    std::vector<NetworkTopology *> thread_local_networks(num_threads);
    std::vector<MultiClassEntropy *> thread_local_outputs(num_threads);


    ParameterContainer *global_param = this->net->current_param_container_ptr;

    for (size_t i = 0; i < num_threads; ++i) {
        thread_local_networks[i] = new NetworkTopology(*net);
        thread_local_networks[i]->set_output(
                new MultiClassEntropy(*dynamic_cast<MultiClassEntropy *>(net->get_output())));
    }

    if (ep_r < 0) {
        ep_r = nbatch;
    }

    std::mutex mtx; // for accessing the shared network object
    std::mutex epoch_time_vector_lock; // for shared times measurements

    // create num_threads x ep matrix for storing thread-local loss sum per epoch
    std::vector<std::vector<Scalar>> local_losses_per_epoch(num_threads);
    std::vector<std::vector<int>> local_epr(num_threads);
    std::vector<std::vector<long>> local_tau_dist(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        for (int j = 0; j < ep_n; ++j) {
            local_losses_per_epoch[i].push_back(0);
            local_epr[i].push_back(0);
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

    bool is_searching = true;
    int active_n = m0 < 0 ? num_threads / 2 : m0;
    int search_hi = num_threads,
        search_lo = 1;
    long state_1st_step = 0;
    int search_depth_done = 0;

    const int max_search_depth = 4;
    const long search_dur = 200;
    const long exec_dur   = 600;

    auto f = [&](int id) {
        while (true) {
            const long s = step.fetch_add(1);
            const long b = next_batch.fetch_add(1) % nbatch;

            if (s >= ep_n * ep_r) {
                return;
            }

            if (is_searching && s - state_1st_step == search_dur) {
                search_depth_done++;

                if (search_depth_done == max_search_depth) {
                    is_searching = false;
                } else {

                }
            }

            const long epoch = s / ep_r;
            const long epoch_s = s % ep_r; 

            auto *local_param = new ParameterContainer(*global_param);

            thread_local_networks[id]->set_pointer(local_param);
            thread_local_networks[id]->forward(x_batches[b]);
            thread_local_networks[id]->backprop(x_batches[b], y_batches[b]);
            const Scalar loss = thread_local_networks[id]->get_loss();

            local_losses_per_epoch[id][epoch] += loss;

            thread_local_networks[id]->set_pointer(global_param);
            delete local_param;

            thread_local_networks[id]->update_cw(thread_local_opts[id]);

            if (epoch_s == ep_r - 1) {
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

    workers.wait_for_all();
    workers.start_all();
    workers.stop();

    std::cout << "Training finished." << std::endl;

    for (int k = 0; k < ep_n; k++) {
        loss = 0;
        for (int i = 0; i < num_threads; i++) {
            loss += local_losses_per_epoch[i][k];
        }
        loss /= ep_r;
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
