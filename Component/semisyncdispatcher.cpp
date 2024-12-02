#include "Component/Dispatcher.hpp"

namespace MiniDNN {

bool SemiSyncDispatcher::try_start_step(long worker_id) {
    if (this->is_finished()) {
        return false;
    } else {
        if (worker_id < this->exec.get_paracontr()->get_parallelism()) {
            return starts_counter.fetch_add(1) < async_period;
        } else {
            return false;
        }
    }
}

long SemiSyncDispatcher::finish_step(long worker_id) {
    this->steps_done++;

    if (ends_counter.fetch_add(1) >= async_period - 1) {
        std::cout << "[SEMISYNC] Resetting counter.\n";
        /* Only once all of the allowed steps in this period have completed do
         * we reset the counters so that new ones may start */
        ends_counter = starts_counter = 0;
    }

    return !this->is_finished();
}

bool SemiSyncDispatcher::is_finished() {
    return this->steps_done >= exec.epoch_target * exec.steps_per_epoch;
}

}
