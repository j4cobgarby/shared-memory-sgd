#include "Component/Dispatcher.hpp"

namespace MiniDNN {

bool AsyncDispatcher::try_start_step(long worker_id) {
    if (this->is_finished()) {
        return false;
    } else {
        return worker_id < this->exec.get_paracontr()->get_parallelism();
    }
}

bool AsyncDispatcher::finish_step(long worker_id) {
    this->steps_done++;
    return !this->is_finished();
}

bool AsyncDispatcher::is_finished() {
    return this->steps_done >= exec.epoch_target * exec.steps_per_epoch;
}

}
