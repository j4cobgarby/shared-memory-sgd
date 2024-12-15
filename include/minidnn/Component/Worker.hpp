#pragma once

#include "NetworkTopology.h"
#include "Optimizer.h"
#include "modular_components.hpp"

#include <barrier>
#include <chrono>
#include <memory>

#define MEASURE_STEP_TIME 1
#define N_STEP_TIME_SAMPLES 30000

namespace MiniDNN {

class SGDWorkerAsync : public Worker {
    typedef std::chrono::high_resolution_clock HRClock;
protected:
    std::unique_ptr<NetworkTopology> network;
    std::unique_ptr<Optimizer> optim;

    std::array<long, MAX_TAU_DIST> _tau_distr = {0};
#if MEASURE_STEP_TIME
    // t_start, t_end, thread_id, tau
    std::vector<std::tuple<long, long, long, long>> steptime_samples;
#endif
public:
    /* pin: hw thread to pin to, or -1 to not pin */
    SGDWorkerAsync(SystemExecutor &exec, long id, std::atomic_flag *flag) : Worker(exec, id, flag) {
        this->network = std::make_unique<NetworkTopology>(*exec.get_model()->get_network());
        this->optim = std::unique_ptr<Optimizer>(exec.get_model()->get_optimizer()->clone());
        set_cpu(id);
#if MEASURE_STEP_TIME
        /* Tuples are (start,end,thread_id) for each step */
        steptime_samples.reserve(N_STEP_TIME_SAMPLES);
#endif
    }

    void run() override;
};

class SGDWorkerSynchronous : public Worker {
protected:
    std::unique_ptr<NetworkTopology> network;
    std::unique_ptr<Optimizer> optim;
#if MEASURE_STEP_TIME
    // t_start, t_end, thread_id, tau
    std::vector<std::tuple<long, long, long, long>> steptime_samples;
#endif
public:
    SGDWorkerSynchronous(SystemExecutor &exec, long id, std::atomic_flag *flag) : Worker(exec, id, flag) {
        this->network = std::make_unique<NetworkTopology>(*exec.get_model()->get_network());
        this->optim = std::unique_ptr<Optimizer>(exec.get_model()->get_optimizer()->clone());
        set_cpu(id);
#if MEASURE_STEP_TIME
        steptime_samples.reserve(N_STEP_TIME_SAMPLES);
#endif
    }

    void run() override;
};

/* Simple WorkerPool that repeatedly (synchronously) runs all the workers, until the dispatcher is done.
 * Asynchrony is achieved via having the worker's run method performing repetion asynchronously.
 */
template <typename WorkerType>
class ThreadWorkerPoolAsync : public WorkerPool {
protected:
    /* Each one of these workers has a corresponding thread at the same index in
     * worker_threads, which runs that worker's run() method with that worker as
     * the `this` argument */
    std::vector<WorkerType> workers;
    std::vector<std::thread *> worker_threads;
    std::atomic_flag workers_flag;
    std::barrier<> loop_sync;
public:
    ThreadWorkerPoolAsync(SystemExecutor &exec, int n_workers, bool pin);

    void wait_for_all() override;
    void start_all() override;
};

}

