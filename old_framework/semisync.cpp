#include "NetworkExecutor.h"
#include "NetworkTopology.h"
#include "Output/MultiClassEntropy.h"
#include "ParameterContainer.h"
#include "ThreadPool.h"
#include "Utils/Random.h"
#include <sys/select.h>
#include <atomic>

void MiniDNN::NetworkExecutor::run_semisync(int batch_size, int num_epochs, int rounds_per_epoch, struct timeval start_time, int sync_interval, int seed) {
    opt->reset();
    if (seed > 0) m_rng.seed(seed);

    std::vector<XType> x_batches;
    std::vector<YType> y_batches;

    const int nbatch = internal::create_shuffled_batches(x, y, batch_size, m_rng, x_batches, y_batches);

    std::vector<NetworkTopology *> thread_networks(num_threads);
    std::vector<MultiClassEntropy *> thread_outputs(num_threads);

    ParameterContainer *global_param = net->current_param_container_ptr;

    for (size_t i = 0; i < num_threads; i++) {
        thread_networks[i] = new NetworkTopology(*net);
        thread_networks[i]->set_output(
            new MultiClassEntropy(*dynamic_cast<MultiClassEntropy *>(net->get_output()))
        );
    }

    if (rounds_per_epoch < 0) rounds_per_epoch = nbatch;

    std::mutex mtx_params; // Mutex for global params
    std::mutex mtx_time; // For the epoch time vector

    std::vector<std::vector<Scalar>> thread_losses_per_epoch(num_threads);
    std::vector<std::vector<long>> thread_tau_dists(num_threads);

    for (int i = 0; i < num_threads; i++) {
        for (int j = 0; j < num_epochs; j++)
            thread_losses_per_epoch[i].push_back(0);

        for (int j = 0; j < tau_threshold; j++)
            thread_tau_dists[i].push_back(0);
    }

    for (int j = 0; j < tau_threshold; j++)
        tau_dist_sample.push_back(0);

    std::atomic<int> next_batch(0);
    std::atomic<long> step(0);

    int num_iterations = sync_interval;
    std::cout << "sync_interval: " << num_iterations << std::endl;
    std::atomic_flag should_stop = ATOMIC_FLAG_INIT;

    auto f = [&](int id) {
        int i;
        for (i = 0; i < num_iterations; i++) {
            if (should_stop.test()) break;
            if (i + 1 == num_iterations) should_stop.test_and_set();

            long local_step = step.fetch_add(1);

            if (tauadaptstrat != "NONE" && local_step == tau_sample_stop * num_threads) {
                compute_tail_dist();
            }

            int batch_index = next_batch.fetch_add(1) % nbatch;

            if (local_step >= num_epochs * rounds_per_epoch)
                break;

            long epoch = local_step / rounds_per_epoch;
            long epoch_step = local_step % rounds_per_epoch;

            mtx_params.lock();
            auto *local_param = new ParameterContainer(*global_param);
            mtx_params.unlock();

            long t0 = local_param->timestamp;

            thread_networks[id]->set_pointer(local_param);
            thread_networks[id]->forward(x_batches[batch_index]);
            thread_networks[id]->backprop(x_batches[batch_index], y_batches[batch_index]);
            const Scalar loss = thread_networks[id]->get_loss();
            thread_losses_per_epoch[id][epoch] += loss;
            thread_networks[id]->set_pointer(global_param);
            delete local_param;

            mtx_params.lock();
            long t1 = global_param->timestamp;

            // Staleness of this update with respect to the model which was
            // fetched.
            int tau = t1 - t0;

            thread_local_opts[id]->step_scale_factor = get_stepsize_scaling_factor(tau, tauadaptstrat);
            thread_networks[id]->update_cw(thread_local_opts[id]);
            mtx_params.unlock();

            if (tau < tau_threshold) {
                thread_tau_dists[id][tau] += 1;
                if (tauadaptstrat != "NONE" && local_step > tau_sample_start * num_threads && local_step < tau_sample_stop * num_threads) {
                    tau_dist_sample[tau] += 1;
                }
            }

            if (epoch_step == rounds_per_epoch - 1) {
                struct timeval now;
                gettimeofday(&now, NULL);
                mtx_time.lock();
                time_per_epoch.push_back(now.tv_sec - start_time.tv_sec);
                mtx_time.unlock();
            }
        }
        // std::cout << "[" << id << "]" << "f() finished after " << i << " iterations\n";
    };

    std::vector<std::function<void(int id)>> jobs;

    for (int i = 0; i < num_threads; i++)
        jobs.push_back(f);

    ThreadPool workers(num_threads, jobs);
    workers.wait_for_all();

    long curr_step;
    while ((curr_step = step.load()) < num_epochs * rounds_per_epoch) {
        std::cout << "====\n";
        should_stop.clear();
        workers.start_all();
        workers.wait_for_all();
    }

    workers.stop();
    std::cout << "Finished training\n";
    for (int k = 0; k < num_epochs; k++) {
        loss = 0;
        for (int i = 0; i < num_threads; i++) {
            loss += thread_losses_per_epoch[i][k];
        }
        loss /= rounds_per_epoch;
        loss_per_epoch.push_back(loss);
    }
    for (int tau = 0; tau < tau_threshold; tau++) {
        int tau_count = 0;
        for (int i = 0; i < num_threads; i++) {
            tau_count += thread_tau_dists[i][tau];
        }
        tau_dist.push_back(tau_count);
    }
}
