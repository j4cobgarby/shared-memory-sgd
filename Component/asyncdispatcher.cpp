#include "Component/Dispatcher.hpp"

namespace MiniDNN {

std::pair<bool, long> AsyncDispatcher::try_start_step(long worker_id) {
    if (this->is_finished()) {
        return {false, 0};
    } else {
        return {worker_id < this->exec.get_paracontr()->get_parallelism(), steps_started.load()};
    }
}

bool AsyncDispatcher::finish_step(long worker_id, long step_ind, long &end_step_ind) {
    end_step_ind = this->steps_done.fetch_add(1);
    return true;
}

bool AsyncDispatcher::is_finished() {
    // return this->exec.elapsed_time() > 1000 * 180;
    return this->steps_done >= exec._epoch_target * exec._steps_per_epoch;
}

}
