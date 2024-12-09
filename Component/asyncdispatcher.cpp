#include "Component/Dispatcher.hpp"

namespace MiniDNN {

bool AsyncDispatcher::try_start_step(long worker_id) {
    if (this->is_finished()) {
        return false;
    } else {
        return worker_id < this->exec.get_paracontr()->get_parallelism();
    }
}

long AsyncDispatcher::finish_step(long worker_id) {
    return this->steps_done.fetch_add(1);
}

bool AsyncDispatcher::is_finished() {
    // return this->exec.elapsed_time() > 1000 * 180;
    return this->steps_done >= exec.epoch_target * exec.steps_per_epoch;
}

}
