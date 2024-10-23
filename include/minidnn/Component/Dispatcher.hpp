#ifndef COMPONENT_DISPATCHER_HPP
#define COMPONENT_DISPATCHER_HPP

#include "modular_components.hpp"

namespace MiniDNN {

class AsyncDispatcher : public Dispatcher {
protected:
    long steps_done = 0;
public:
    AsyncDispatcher(SystemExecutor &exec) : Dispatcher(exec) {}

    bool try_start_step(long worker_id) override;
    bool finish_step(long worker_id) override;
    bool is_finished() override;
};

}

#endif /* COMPONENT_DISPATCHER_HPP */
