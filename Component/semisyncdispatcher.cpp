#include "Component/Dispatcher.hpp"

namespace MiniDNN {

bool SemiSyncDispatcher::try_start_step(long worker_id) {
    if (this->is_finished()) {
        return false;
    } else {
        if (worker_id < this->exec.get_paracontr()->get_parallelism()) {
            return starts_counter++ < async_period;
        } else {
            return false;
        }
    }
}

bool SemiSyncDispatcher::finish_step(long worker_id) {
    if (++ends_counter >= async_period) {
        /* Only once all of the allowed steps in this period have completed do
         * we reset the counters so that new ones may start */
        ends_counter = starts_counter = 0;
    }

    if (this->is_finished()) {
        return false;
    } else {
        this->steps_done++;
        return true;
    }
}

bool SemiSyncDispatcher::is_finished() {
    return this->steps_done >= exec.epoch_target * exec.steps_per_epoch;
}

}
