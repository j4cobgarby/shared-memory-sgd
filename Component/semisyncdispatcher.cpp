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

bool SemiSyncDispatcher::finish_step(const long worker_id, const long step_ind, long &end_step_ind) {

    // If this step begun before this period, reject it
    if (step_ind < period_start_step.load(std::memory_order_acquire)) {
        return false;
    }

    long old_val, new_val;
    do {
        old_val = steps_done_in_period.load(std::memory_order_relaxed);

        // Reject any more steps than the period allows
        if (old_val >= async_period || step_ind < period_start_step.load(std::memory_order_acquire)) {
            return false;
        }

        new_val = old_val + 1;
    } while (!steps_done_in_period.compare_exchange_weak(old_val, new_val,
            std::memory_order::acquire, std::memory_order::relaxed));

    if (new_val == async_period) {
        period_start_step.store(steps_started, std::memory_order::release);
        steps_done_in_period.store(0, std::memory_order::release);
    }

    end_step_ind = this->steps_done.fetch_add(1);

    return true;
}

bool SemiSyncDispatcher::is_finished() {
    // return this->exec.elapsed_time() >= 1000 * 180;
    return this->steps_done >= exec._epoch_target * exec._steps_per_epoch;
}

}
