#ifndef NETWORKEXECUTOR_H_
#define NETWORKEXECUTOR_H_

#include <bits/types/struct_timeval.h>
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <sys/select.h>

#include "ParameterContainer.h"
#include "ThreadPool.h"
#include "NetworkTopology.h"
#include "Output/MultiClassEntropy.h"

namespace MiniDNN {
    class NetworkExecutor {
        typedef Matrix DerivedX;
        typedef Matrix DerivedY;
        typedef typename Eigen::MatrixBase<Matrix>::PlainObject PlainObjectX;
        typedef typename Eigen::MatrixBase<Matrix>::PlainObject PlainObjectY;
        typedef Eigen::Matrix<typename PlainObjectX::Scalar, PlainObjectX::RowsAtCompileTime, PlainObjectX::ColsAtCompileTime>
                XType;
        typedef Eigen::Matrix<typename PlainObjectY::Scalar, PlainObjectY::RowsAtCompileTime, PlainObjectY::ColsAtCompileTime>
                YType;

    private:
        Optimizer *opt;
        std::vector<Optimizer *> thread_local_opts;
        const Eigen::MatrixBase<Matrix> &x;
        const Eigen::MatrixBase<Matrix> &y;
        RNG m_default_rng;      // Built-in RNG
        RNG &m_rng;              // Reference to the RNG provided by the user,
        // otherwise reference to m_default_rng
        NetworkTopology *net;
        Scalar loss = 0;
        Scalar prev_loss = NAN;
        std::vector<Scalar> loss_per_epoch;
        std::vector<double> time_per_epoch;

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
        std::vector<int> m_probe_starts;
        std::vector<int> m_probe_ends;
        std::vector<int> m_exec_values;


        struct timeval exe_start;

    public:
        double scalar_loss_grad, scalar_loss_jitter, scalar_m_trend;

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

        void run_training(int, int, int, int seed = -1);
        void run_parallel_sync(int, int, int, int seed = -1);
        void run_parallel_async(int, int, int, struct timeval, int seed = -1, bool use_lock = true);
        void run_parallel_leashed(int, int, int, int, bool, int seed = -1);
        void run_elastic_async(int, int, int, int, int, int, int, struct timeval, int seed = -1, bool use_lock=true);
        void run_semisync(int batch_size, int num_epochs, int rounds_per_epoch, struct timeval start_time, int sync_interval, int seed = -1);
        void run_heuristic_async(int batch_size, int num_epochs, int rounds_per_epoch, double grad, struct timeval start_time, int seed);
        void run_elastic_async2(int, int, int, int, int, int, int, int, bool);
        void run_search_probing(int bs, int ep, int epr, int m0, struct timeval start_time, int seed);

        void set_threads_running(std::atomic_flag arr[], int n, int num_threads);

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

        std::vector<double> &get_times_per_epoch() {
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

        std::vector<int> &get_probe_starts() {
            return m_probe_starts;
        }

        std::vector<int> &get_probe_ends() {
            return m_probe_ends;
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
            return 1.0;
        }

        void update_loss_grad(double this_loss, struct timeval &start_time) {
            if (std::isnan(prev_loss)) prev_loss = this_loss;
            double prev_grad_ema = loss_grads.empty() ? 0 : loss_grads.back();
            loss_grads.push_back(this_loss - prev_loss);
            prev_loss = this_loss;

            struct timeval now;
            gettimeofday(&now, NULL);
            loss_grad_times.push_back((double)(now.tv_sec - start_time.tv_sec) + (double)(now.tv_usec - start_time.tv_usec)/1000000);
        }

        ~NetworkExecutor() {}
    };
}
#endif /* NETWORKEXECUTOR_H_ */
