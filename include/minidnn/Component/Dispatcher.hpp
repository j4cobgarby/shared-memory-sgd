#ifndef COMPONENT_DISPATCHER_HPP
#define COMPONENT_DISPATCHER_HPP

#include "modular_components.hpp"

namespace MiniDNN {

class AsyncDispatcher : public Dispatcher {
public:
    AsyncDispatcher(SystemExecutor &exec) : Dispatcher(exec) {}

    bool try_start_step(long worker_id) override;
    bool finish_step(long worker_id) override;
    bool is_finished() override;
};

class SemiSyncDispatcher : public Dispatcher {
private:
    /* Number of steps to complete before synchronising */
    const long async_period;

    /* Counts how many steps have been started in this async period */
    long starts_counter = 0;

    /* Counts how many steps have concluded in this period */
    long ends_counter = 0;
public:
    SemiSyncDispatcher(SystemExecutor &exec, long P) :
        Dispatcher(exec),
        async_period(P) {}

    bool try_start_step(long worker_id) override;
    bool finish_step(long worker_id) override;
    bool is_finished() override;
};

}

#endif /* COMPONENT_DISPATCHER_HPP */
