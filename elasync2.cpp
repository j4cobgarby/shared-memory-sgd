#include "NetworkExecutor.h"
#include "ParameterContainer.h"
#include "ThreadPool.h"
#include <array>
#include <atomic>
#include <condition_variable>
#include <limits>
#include <mutex>
#include <sys/select.h>
#include <chrono>

namespace MiniDNN {

void NetworkExecutor::set_threads_running(std::atomic_flag arr[], int n, int num_threads) {
    for (int i = 0; i < num_threads; i++) {
        if (i <= n) {
            arr[i].test_and_set();
        } else {
            arr[i].clear();
        }
    }
    m_values.push_back(n);
    struct timeval t_now;
    gettimeofday(&t_now, NULL);
    m_times.push_back((double)(t_now.tv_sec - exe_start.tv_sec) + (double)(t_now.tv_usec - exe_start.tv_usec)/1000000);
}

void NetworkExecutor::run_elastic_async2(int batch_size, int num_epochs, int rounds_per_epoch, int window, int probing_interval,
                                         int probing_duration, int m_0, int seed, bool use_lock) {
    use_lock = false;
    opt->reset();

    if (seed > 0) m_rng.seed(seed);

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

    std::atomic_flag all_stop = ATOMIC_FLAG_INIT;
    std::atomic_flag thread_should_run[num_threads];
    int thread_model_ids[num_threads];

    std::cout << "Initial setting of threads <= " << current_parallelism << std::endl;
    for (int i = 0; i < num_threads; i++) {
        if (i <= current_parallelism) {
            thread_should_run[i].test_and_set();
        } else {
            thread_should_run[i].clear();
        }
    }
    set_threads_running(thread_should_run, current_parallelism, num_threads);

    int  phase_firststep = 0;
    bool in_probing = false;
    int probing_start, probing_end, probing_step, probing_current;
    std::chrono::time_point<std::chrono::steady_clock> probe_step_starttime;
    double best_loss = std::numeric_limits<double>::infinity();
    int best_parallelism;

    auto f = [&](int id) {
        while (true) {
            if (all_stop.test()) {
                return;
            }

            if (thread_should_run[id].test()) {
                for (int i = 0; i < 1; i++) {
                    long local_step = step.fetch_add(1);
                    int batch_index = next_batch.fetch_add(1) % nbatch;
                    long epoch = local_step / rounds_per_epoch;
                    long epoch_step = local_step % rounds_per_epoch;

                    if (local_step >= num_epochs * rounds_per_epoch) {
                        std::cout << "Finished! Instructing all to stop\n";
                        all_stop.test_and_set();
                        return;
                    }

                    /* The following block will check if the current thread is in the situation
                     * where it needs to switch the global execution state, i.e. switch from probing phase
                     * to execution phase, or to change the parallelism level in the probing phase. */
                    if (in_probing) {
                        if ((local_step - phase_firststep) % 32 == 0) {
                            /* At this point in time, thread_local_networks[i] has the global model as its model,
                             * so this will be the current global model */
                            double this_loss = thread_local_networks[i]->get_loss();
                            std::cout << "Parallelism of " << current_parallelism << " yielded loss of " << this_loss << std::endl;
                            if (this_loss < best_loss) {
                                best_loss = this_loss;
                                best_parallelism = current_parallelism;
                            }

                            if (current_parallelism < probing_end) {
                                current_parallelism += probing_step;

                                std::cout << "Setting thread statuses <= " << current_parallelism << std::endl;
                                set_threads_running(thread_should_run, current_parallelism, num_threads);

                                probe_step_starttime = std::chrono::steady_clock::now();
                            } else {
                                /* Finished probing */
                                std::cout << "Finished probing, found best parallelism = " << best_parallelism << std::endl;
                                current_parallelism = best_parallelism;
                                std::cout << "(Starting execution phase) Setting thread statuses <= " << current_parallelism << std::endl;
                                set_threads_running(thread_should_run, current_parallelism, num_threads);
                                in_probing = false;
                            }
                        }
                    } else {
                        if ((local_step - phase_firststep) % probing_interval == 0) {
                            in_probing = true;
                            probing_step = 1;
                            probing_start = current_parallelism - window/2;
                            probing_end = probing_start + window - 1;

                            if (probing_start < 1) probing_start = 1;
                            if (probing_end > num_threads) probing_end = num_threads;

                            phase_firststep = local_step;
                            best_loss = std::numeric_limits<double>::infinity(); 
                            best_parallelism = -1;

                            current_parallelism = probing_start;

                            std::cout << "(Starting new probing phase) Setting thread statuses <= " << current_parallelism << std::endl;
                            set_threads_running(thread_should_run, current_parallelism, num_threads);

                            probe_step_starttime = std::chrono::steady_clock::now();
                        }
                    }


                    mtx.lock();
                    auto *local_param = new ParameterContainer(*global_param);
                    mtx.unlock();

                    thread_local_networks[id]->set_pointer(local_param);
                    thread_local_networks[id]->forward(x_batches[batch_index]);
                    thread_local_networks[id]->backprop(x_batches[batch_index], y_batches[batch_index]);

                    const Scalar loss = thread_local_networks[id]->get_loss();
                    local_losses_per_epoch[id][epoch] += loss;
                    std::cout << "EPOCH LOSS " << epoch << " += " << loss << std::endl;
                    thread_local_networks[id]->set_pointer(global_param);

                    delete local_param;

                    /* Integrate gradient into global model */
                    mtx.lock();
                    thread_local_opts[id]->step_scale_factor = 1.0;
                    std::cout << "Updating weights (id = " << id << ")\n";
                    thread_local_networks[id]->update_cw(thread_local_opts[id]);
                    mtx.unlock();

                    if (epoch_step == rounds_per_epoch - 1) {
                        struct timeval now;
                        gettimeofday(&now, NULL);

                        time_per_epoch.push_back(now.tv_sec - exe_start.tv_sec + (double)(now.tv_usec - exe_start.tv_usec) / 1000000);
                    }
                }
            } else {
                /* If we can't run right now, then wait until we can */
                //thread_should_run[id].wait(false);
            }
        }
    };

    std::vector<std::function<void (int id)>> jobs;
    for (int i = 0; i < num_threads; i++) {
        jobs.push_back(f);
    }
    ThreadPool workers(num_threads, jobs);
    std::cout << "Made pool of " << num_threads << " workers\n";

    gettimeofday(&this->exe_start, NULL);

    workers.wait_for_all();
    workers.start_all();
    workers.wait_for_all();

    std::cout << "All done\n";
    workers.stop();
}

}
