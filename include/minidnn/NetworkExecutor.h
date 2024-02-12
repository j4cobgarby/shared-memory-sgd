#ifndef NETWORKEXECUTOR_H_
#define NETWORKEXECUTOR_H_

#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <sys/select.h>

#include "ParameterContainer.h"
#include "ThreadPool.h"
#include "NetworkTopology.h"

namespace MiniDNN {
    template<typename DerivedX, typename DerivedY>
    class NetworkExecutor {

        typedef typename Eigen::MatrixBase<DerivedX>::PlainObject PlainObjectX;
        typedef typename Eigen::MatrixBase<DerivedY>::PlainObject PlainObjectY;
        typedef Eigen::Matrix<typename PlainObjectX::Scalar, PlainObjectX::RowsAtCompileTime, PlainObjectX::ColsAtCompileTime>
                XType;
        typedef Eigen::Matrix<typename PlainObjectY::Scalar, PlainObjectY::RowsAtCompileTime, PlainObjectY::ColsAtCompileTime>
                YType;

    private:
        Optimizer *opt;
        std::vector<Optimizer *> thread_local_opts;
        const Eigen::MatrixBase<DerivedX> &x;
        const Eigen::MatrixBase<DerivedY> &y;
        RNG m_default_rng;      // Built-in RNG
        RNG &m_rng;              // Reference to the RNG provided by the user,
        // otherwise reference to m_default_rng
        NetworkTopology *net;
        Scalar loss = 0;
        Scalar prev_loss = NAN;
        std::vector<Scalar> loss_per_epoch;
        std::vector<long> time_per_epoch;

        int tau_threshold = 200;
        std::vector<double> tau_dist;

        // adaptation amplitude when using the TAIL step size function
        float adaptation_amplitude = 1.0;

        // parameters for collecting a representative tau distribution in the beginning
        int tau_sample_start = 10; // n.o. iterations per thread before tau collection starts
        int tau_sample_stop = 15; // n.o. iterations per thread when tau collection ends
        std::vector<float> tau_dist_sample; // representative tau distribution, used to compute tail_dist
        std::vector<float> tail_dist; // tail distribution (CDF)
        bool tail_dist_finished = false; // indicates wether the tail distribution has been computed yet

        std::vector<double> num_tries_dist;

        std::string tauadaptstrat;
        double base_stepsize;
        int num_threads;
        int algorithm_id;
        int arch_id;  // 0: MLP, 1: CNN

        int max_staleness = 199;

        std::vector<double> loss_grads;
        std::vector<double> loss_grad_times;
        std::vector<double> m_times;
        std::vector<int> m_values;

    public:

        long failed_cas = 0;

        /// \param opt        An object that inherits from the Optimizer class, indicating the optimization algorithm to use.
        /// \param x          The predictors. Each column is an observation.
        /// \param y          The response variable. Each column is an observation.
        NetworkExecutor(NetworkTopology *_net, Optimizer *_opt, std::vector<Optimizer *> _thread_local_opts, const Eigen::MatrixBase<DerivedX> &_x,
                        const Eigen::MatrixBase<DerivedY> &_y, std::string _tauadaptstrat, int _num_threads, double _base_stepsize, int _algorithm_id, int _arch_id)
                : m_default_rng(1),
                  m_rng(m_default_rng),
                  thread_local_opts(std::move(_thread_local_opts)),
                  opt(_opt),
                  x(_x),
                  y(_y),
                  tauadaptstrat(std::move(_tauadaptstrat)),
                  num_threads(_num_threads),
                  base_stepsize(_base_stepsize),
                  algorithm_id(_algorithm_id),
                  arch_id(_arch_id),
                  net(_net) {
        }

        Scalar get_loss() {
            return loss;
        }

        std::vector<double> &get_loss_grads() {
            return loss_grads;
        }

        std::vector<double> &get_loss_grad_times() {
            return loss_grad_times;
        }

        std::vector<Scalar> &get_losses_per_epoch() {
            return loss_per_epoch;
        }

        Scalar &get_last_epoch_loss() {
            return loss_per_epoch.back();
        }

        std::vector<long> &get_times_per_epoch() {
            return time_per_epoch;
        }

        std::vector<double> &get_tau_dist() {
            return tau_dist;
        }

        std::vector<double> &get_num_tries_dist() {
            return num_tries_dist;
        }

        std::vector<double> &get_m_times() {
            return m_times;
        }

        std::vector<int> &get_m_values() {
            return m_values;
        }

        void compute_tail_dist() {
            long cum_tau = 0;
            int num_steps_used = num_threads * (tau_sample_stop - tau_sample_start);
            for (int j = 0; j < tau_threshold; ++j) {
                tail_dist.push_back( ((float)cum_tau / (float)num_steps_used));
                cum_tau += tau_dist_sample[j];
            }
            tail_dist_finished = true;
        }

        float get_stepsize_scaling_factor(int staleness, const std::string& strategy) {
            if (staleness >> max_staleness)
                staleness = max_staleness;
            if (strategy == "NONE") {
                return 1.0;
            } else if (strategy == "INVERSE") {
                return 1.0 / ((double)staleness+1);
            } else if (strategy == "TAIL") {  // TAIL
                if (tail_dist_finished) {
                    float scaling_factor = 1 + adaptation_amplitude * (1 - 2*tail_dist[staleness]);
                    return scaling_factor;
                } else {
                    return 1.0;
                }
            } else if (strategy == "FLEET") {
                int tau_thresh = num_threads + 1;
                float beta = 2.0 / tau_thresh * log(1 + tau_thresh/2);
                float scaling_factor = exp( - beta * staleness );
                return scaling_factor;
            }
        }

        /// \param epoch      Number of epochs of training.
        /// \param seed       Set the random seed of the %RNG if `seed > 0`, otherwise
        ///                   use the current random state.
        //
        void run_training(int batch_size, int epoch, int rounds_per_epoch, int seed = -1) {

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

            if (rounds_per_epoch < 0) {
                rounds_per_epoch = nbatch;
            }

            long step = 0;

            for (int k = 0; k < epoch; k++) {

                Scalar epoch_loss = 0;

                for (int round = 0; round < rounds_per_epoch; ++round) {
                    long batch_index = step % nbatch;
                    net->forward(x_batches[batch_index]);
                    net->backprop(x_batches[batch_index], y_batches[batch_index]);
                    const Scalar loss = net->get_loss();
                    //std::cerr << "[Step " << step << "] Loss = " << loss << std::endl;

                    epoch_loss += loss;

                    //net->update(opt); // AlignedMapVec update
                    net->update_cw(opt); // component-wise update

                    step++;
                }

                epoch_loss /= rounds_per_epoch;
                loss_per_epoch.push_back(epoch_loss);

                struct timeval now;
                gettimeofday(&now, NULL);
                time_per_epoch.push_back(now.tv_sec);

                //std::cerr << "[Epoch " << k << "] Loss = " << epoch_loss << std::endl;

            }
        }

        
        void run_parallel_sync(int batch_size, int epoch, int rounds_per_epoch, int seed = -1) {

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

            if (rounds_per_epoch < 0) {
                rounds_per_epoch = nbatch;
            }

            std::vector<NetworkTopology *> thread_local_networks(num_threads);
            std::vector<MultiClassEntropy *> thread_local_outputs(num_threads);

            for (size_t i = 0; i < num_threads; ++i) {
                thread_local_networks[i] = new NetworkTopology(*net);
                thread_local_networks[i]->set_output(
                        new MultiClassEntropy(*dynamic_cast<MultiClassEntropy *>(net->get_output())));
            }

            long step = 0;

            auto f = [&](int id) {
                long batch_index = (step * num_threads + id) % nbatch;
                thread_local_networks[id]->forward(x_batches[batch_index]);
                thread_local_networks[id]->backprop(x_batches[batch_index], y_batches[batch_index]);
                const Scalar loss = thread_local_networks[id]->get_loss();
            };

            std::vector<std::function<void(int id)>> jobs;

            for (int i = 0; i < num_threads; ++i) {
                jobs.push_back(f);
            }

            ThreadPool workers(num_threads, jobs);

            //std::cout << "wait for workers" << std::endl;
            workers.wait_for_all();
            //std::cout << "start training" << std::endl;

            // Iterations on the whole data set
            for (int k; k < epoch; k++) {

                Scalar epoch_loss = 0;

                for (int round = 0; round < rounds_per_epoch; ++round) {

                    // Train on each mini-batch
                    workers.start_all();
                    workers.wait_for_all();

                    Scalar round_loss = 0;

                    for (auto sub_net: thread_local_networks) {
                        round_loss += sub_net->get_loss();

                        net->reset();
                        net->aggregate(*sub_net);
                        net->normalize_derivatives(num_threads);

                        //net->update(opt); // AlignedMapVec update
                        net->update_cw(opt); // component-wise update
                    }

                    step++;

                    round_loss /= num_threads;

                    std::cerr << "[Step " << step << "] Loss = " << round_loss << std::endl;

                    epoch_loss += round_loss;

                    std::vector<std::vector<Scalar>> agg_params = net->get_parameters();

                    for (auto sub_net: thread_local_networks) {
                        sub_net->set_parameters(agg_params);
                    }

                }
                epoch_loss /= rounds_per_epoch;
                loss_per_epoch.push_back(epoch_loss);

                struct timeval now;
                gettimeofday(&now, NULL);
                time_per_epoch.push_back(now.tv_sec);

                //std::cerr << "[Epoch " << k << "] Loss = " << epoch_loss << std::endl;

            }

            workers.stop();
            loss = 0;
            for (int i = 0; i < num_threads; i++) {
                loss += thread_local_networks[i]->get_loss();
            }
            loss /= num_threads;
        }

        double compute_loss_ema(double acc, double this_loss, double alpha) {
            return alpha * this_loss + (1 - alpha) * acc;
        }

        void update_loss_grad(double this_loss, struct timeval &start_time) {
            if (std::isnan(prev_loss)) prev_loss = this_loss;
            double prev_grad_ema = loss_grads.empty() ? 0 : loss_grads.back();
            /* double to_push = compute_loss_ema(prev_grad_ema, this_loss - prev_loss, 0.3);  */
            loss_grads.push_back(this_loss - prev_loss);
            prev_loss = this_loss;

            struct timeval now;
            gettimeofday(&now, NULL);
            loss_grad_times.push_back((double)(now.tv_sec - start_time.tv_sec) + (double)(now.tv_usec - start_time.tv_usec)/1000000);
        }

        void run_elastic_async(int batch_size, int num_epochs, int rounds_per_epoch, int window, int probing_interval, int probing_duration, int m_0, struct timeval start_time, int seed = -1, bool use_lock=true) {
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

                // double &grad_ema = thread_gradients[id];
                // double &prev_loss = thread_prev_losses[id];
                //
                // if (prev_loss == NAN) prev_loss = loss;
                //
                // grad_ema = compute_loss_ema(grad_ema, loss - prev_loss, 0.3);
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
        
        void run_parallel_async(int batch_size, int num_epochs, int rounds_per_epoch, int seed = -1, bool use_lock = false) {

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
                        if (!time_per_epoch.empty()) {
                            if (time_per_epoch.back() > now.tv_sec)
                                std::cout << "WARNING: inconsistent epoch time values" << std::endl;
                        }
                        time_per_epoch.push_back(now.tv_sec);
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

        void run_parallel_leashed(int batch_size, int num_epochs, int rounds_per_epoch, int CAS_backoff, bool check_concurrent_updates, int seed = -1) {

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

        ~NetworkExecutor() {}

    };
}
#endif /* NETWORKEXECUTOR_H_ */
