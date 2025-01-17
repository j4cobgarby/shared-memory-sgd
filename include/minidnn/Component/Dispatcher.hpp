#ifndef COMPONENT_DISPATCHER_HPP
#define COMPONENT_DISPATCHER_HPP

#include "modular_components.hpp"
#include <atomic>

namespace MiniDNN {

class AsyncDispatcher : public Dispatcher {
public:
    AsyncDispatcher(SystemExecutor &exec) : Dispatcher(exec) {}

    std::pair<bool, long> try_start_step(long worker_id) override;
    bool finish_step(long worker_id, long step_ind, long &end_step_ind) override;
    bool is_finished() override;
};

class SemiSyncDispatcher : public Dispatcher {
private:
    long async_period; /* Number of steps to complete before synchronising */
    long async_period_min;

    int period_reduce_counter = 0;
    int period_reduce_step;
    int period_reduce_period;

    std::atomic<long> period_start_step = 0;
    std::atomic<long> steps_done_in_period = 0;

    // std::condition_variable cv;
    // std::mutex cv_mtx;
public:
    SemiSyncDispatcher(SystemExecutor &exec, long P_0, long P_min, int reduce_period, int reduce_step) :
        Dispatcher(exec),
        async_period(P_0), async_period_min(P_min),
        period_reduce_step(reduce_step), period_reduce_period(reduce_period) {}

    std::pair<bool, long> try_start_step(long worker_id) override;
    bool finish_step(long worker_id, long step_ind, long &end_step_ind) override;
    bool is_finished() override;
};

class ResettingDispatcher : public Dispatcher {
public:
    explicit ResettingDispatcher(SystemExecutor& exec)
        : Dispatcher(exec)
    {
    }

    std::pair<bool, long> try_start_step(long worker_id) override;
    bool finish_step(long worker_id, long step_ind, long &end_step_ind) override;
    bool is_finished() override;
};

}

#endif /* COMPONENT_DISPATCHER_HPP */
