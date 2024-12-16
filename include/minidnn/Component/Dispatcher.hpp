#ifndef COMPONENT_DISPATCHER_HPP
#define COMPONENT_DISPATCHER_HPP

#include "modular_components.hpp"
#include <atomic>
#include <condition_variable>

namespace MiniDNN {

class AsyncDispatcher : public Dispatcher {
public:
    AsyncDispatcher(SystemExecutor &exec) : Dispatcher(exec) {}

    std::pair<bool, long> try_start_step(long worker_id) override;
    bool finish_step(long worker_id, long step_ind) override;
    bool is_finished() override;
};

class SemiSyncDispatcher : public Dispatcher {
private:
    /* Number of steps to complete before synchronising */
    long async_period;

    std::atomic<long> period_start_step = 0;

    std::condition_variable cv;
    std::mutex cv_mtx;
public:
    SemiSyncDispatcher(SystemExecutor &exec, long P) :
        Dispatcher(exec),
        async_period(P) {}

    std::pair<bool, long> try_start_step(long worker_id) override;
    bool finish_step(long worker_id, long step_ind) override;
    bool is_finished() override;
};

class ResettingDispatcher : public Dispatcher {
public:
    explicit ResettingDispatcher(SystemExecutor& exec)
        : Dispatcher(exec)
    {
    }

    std::pair<bool, long> try_start_step(long worker_id) override;
    bool finish_step(long worker_id, long step_ind) override;
    bool is_finished() override;
};

}

#endif /* COMPONENT_DISPATCHER_HPP */
