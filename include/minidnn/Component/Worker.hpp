#pragma once

#include "NetworkTopology.h"
#include "modular_components.hpp"

namespace MiniDNN {

class SGDThread : public Worker {
protected:
    NetworkTopology network;
public:
    /* pin: hw thread to pin to, or -1 to not pin */
    SGDThread(SystemExecutor &exec, long id, int pin) : Worker(exec, id) {}

    void run() override;
};

/* Simple WorkerPool that specifically deals with SGDThread Workers */
class ThreadWorkerPool : public WorkerPool {
protected:
    std::vector<std::unique_ptr<SGDThread>> workers;
public:
    ThreadWorkerPool(SystemExecutor &exec, int n_workers) : WorkerPool(exec, n_workers) {}
};

}
