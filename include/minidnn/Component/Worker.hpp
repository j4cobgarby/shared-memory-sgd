#pragma once

#include "NetworkTopology.h"
#include "Optimizer.h"
#include "modular_components.hpp"
#include <chrono>
#include <memory>

#define MEASURE_STEP_TIME 1
#define N_STEP_TIME_SAMPLES 15000

namespace MiniDNN {

class SGDWorker : public Worker {
    typedef std::chrono::high_resolution_clock HRClock;
protected:
    std::unique_ptr<NetworkTopology> network;
    std::unique_ptr<Optimizer> optim;

#if MEASURE_STEP_TIME
    std::vector<std::tuple<long, long>> steptime_samples;
#endif
public:
    /* pin: hw thread to pin to, or -1 to not pin */
    SGDWorker(SystemExecutor &exec, long id, std::atomic_flag *flag) : Worker(exec, id, flag) {
        this->network = std::make_unique<NetworkTopology>(*exec.get_model()->get_network());
        this->optim = std::unique_ptr<Optimizer>(exec.get_model()->get_optimizer()->clone());

#if MEASURE_STEP_TIME
        /* Tuples are (start,end) time of each step */

        steptime_samples.reserve(N_STEP_TIME_SAMPLES);
#endif
        set_cpu(id);
    }

    void run() override;
};

/* Simple WorkerPool that specifically makes threads for its workers */
template <typename WorkerType>
class ThreadWorkerPool : public WorkerPool {
protected:
    /* Each one of these workers has a corresponding thread at the same index in
     * worker_threads, which runs that worker's run() method with that worker as
     * the `this` argument */
    std::vector<WorkerType> workers;
    std::vector<std::thread *> worker_threads;
    std::atomic_flag workers_flag;
public:
    ThreadWorkerPool(SystemExecutor &exec, int n_workers, bool pin);

    void wait_for_all() override;
    void start_all() override;
};

}
