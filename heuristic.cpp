#include "NetworkExecutor.h"
#include "ParameterContainer.h"

#include <algorithm> // std::max
#include <sys/select.h>

#define HEUR_DO_LOCK 0

void MiniDNN::NetworkExecutor::run_heuristic_async(int batch_size, int num_epochs, int rounds_per_epoch, double grad, struct timeval start_time, int seed) {
    opt->reset();

    if (seed > 0) m_rng.seed(seed);

    std::vector<XType> x_batches;
    std::vector<YType> y_batches;

    const int nbatch = internal::create_shuffled_batches(x, y, batch_size, m_rng, x_batches, y_batches);

    std::vector<NetworkTopology *> thread_local_networks(num_threads);
    std::vector<MultiClassEntropy *> thread_local_outputs(num_threads);

    int current_parallelism = num_threads;

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

    const int num_iterations = 100;

    std::atomic_flag should_stop = ATOMIC_FLAG_INIT;

    auto f = [&](int id) {
        int local_iterations = 0;
        if (id >= current_parallelism) return;
        std::cout << "Target = " << num_iterations << std::endl;

        while (!should_stop.test()) {
            if (local_iterations++ >= num_iterations) {
                should_stop.test_and_set();
                std::cout << "Stopping!" << std::endl;
                break;
            }

            long local_step = step.fetch_add(1);
            std::cout << local_step << std::endl;
            int batch_index = next_batch.fetch_add(1) % nbatch;

            if (local_step >= num_epochs * rounds_per_epoch) break;

            long epoch = local_step / rounds_per_epoch;
            long epoch_step = local_step % rounds_per_epoch;

#if HEUR_DO_LOCK
            mtx.lock();
#endif
            auto *local_param = new ParameterContainer(*global_param);
#if HEUR_DO_LOCK
            mtx.unlock();
#endif

            thread_local_networks[id]->set_pointer(local_param);
            thread_local_networks[id]->forward(x_batches[batch_index]);
            thread_local_networks[id]->backprop(x_batches[batch_index], y_batches[batch_index]);
            const Scalar loss = thread_local_networks[id]->get_loss();

            local_losses_per_epoch[id][epoch] += loss;
            thread_local_networks[id]->set_pointer(global_param);

            delete local_param;

#if HEUR_DO_LOCK
            mtx.lock();
#endif
            thread_local_opts[id]->step_scale_factor = 1.0;
            thread_local_networks[id]->update_cw(thread_local_opts[id]);
#if HEUR_DO_LOCK
            mtx.unlock();
#endif

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
    for (int i = 0; i < num_threads; i++) jobs.push_back(f);

    ThreadPool workers(num_threads, jobs);

    workers.wait_for_all();

    long curr_step;
    
    double m_gradient = grad;
    double analogue_m = current_parallelism;
    int last_step = 0;

    std::cout << "Total steps = " << num_epochs << "×" << rounds_per_epoch << " = " << num_epochs * rounds_per_epoch << std::endl;
    while ((curr_step = step.load()) < num_epochs * rounds_per_epoch) {
        /* if (current_parallelism == 30) { */
        /*     std::cout << "Switching gradient" << std::endl; */
        /*     m_gradient = 0.0001; */
        /* } */
        /* current_parallelism = std::max(10.0, num_threads - (curr_step * m_gradient)); */

        /* if (m_gradient == 0.0002 && current_parallelism == 30) { */
        /*     std::cout << "Switching gradient" << std::endl; */
        /*     m_gradient = 0.0001; */
        /* } */

        current_parallelism = std::max(10, (int)analogue_m);
        analogue_m -= m_gradient * (curr_step - last_step);
        last_step = curr_step;
        std::cout << curr_step << ": Parallelism = " << current_parallelism << std::endl;

        struct timeval now;
        gettimeofday(&now, NULL);

        m_values.push_back(current_parallelism);
        m_times.push_back((double)(now.tv_sec - start_time.tv_sec) + (double)(now.tv_usec - start_time.tv_usec)/1000000); 

        should_stop.clear();
        workers.start_all();
        workers.wait_for_all();
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
}
