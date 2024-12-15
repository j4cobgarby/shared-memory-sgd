#include "Component/Dispatcher.hpp"

namespace MiniDNN {

std::pair<bool, long> SemiSyncDispatcher::try_start_step(long worker_id) {
    if (this->is_finished())
        return {false, 0};

    return {worker_id < this->exec.get_paracontr()->get_parallelism(), steps_started.fetch_add(1)};

    // std::unique_lock lock(cv_mtx);
    // cv.wait(lock, [&] { return worker_id < this->exec.get_paracontr()->get_parallelism() && star } );
    //
    // // By this point we know we can start a step, since we waited for the signal and checked the
    // // condition
    // return true;
}

bool SemiSyncDispatcher::finish_step(long worker_id, long step_ind) {
    if (step_ind < period_start_step) return false;

    if (step_ind == period_last_step) {
        period_start_step = step_ind;
        period_last_step = period_start_step + (async_period - 1);
    }

    steps_done.fetch_add(1);
    return true;
}

bool SemiSyncDispatcher::is_finished() {
    // return this->exec.elapsed_time() >= 1000 * 180;
    return this->steps_done >= exec.epoch_target * exec.steps_per_epoch;
}

}
