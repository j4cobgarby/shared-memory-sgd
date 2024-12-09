#include "Component/Dispatcher.hpp"

namespace MiniDNN {

bool SemiSyncDispatcher::try_start_step(long worker_id) {
    if (this->is_finished())
        return false;

    if (worker_id < this->exec.get_paracontr()->get_parallelism()) {
        long curr = starts_counter.load();
        while (curr < async_period) {
            if (starts_counter.compare_exchange_weak(curr, curr + 1)) {
                return true;
            }
        }
        return false;
    }

    return false;

    // std::unique_lock lock(cv_mtx);
    // cv.wait(lock, [&] { return worker_id < this->exec.get_paracontr()->get_parallelism() && star } );
    //
    // // By this point we know we can start a step, since we waited for the signal and checked the
    // // condition
    // return true;
}

long SemiSyncDispatcher::finish_step(long worker_id) {
    const long ret = this->steps_done.fetch_add(1);

    if (ends_counter.fetch_add(1) == async_period - 1) {
        /* Only once all of the allowed steps in this period have completed do
         * we reset the counters so that new ones may start */
        ends_counter = starts_counter = 0;

        // Heuristic for a good period
        async_period = this->exec.get_paracontr()->get_parallelism() / 2;
    }

    return ret;
}

bool SemiSyncDispatcher::is_finished() {
    // return this->exec.elapsed_time() >= 1000 * 180;
    return this->steps_done >= exec.epoch_target * exec.steps_per_epoch;
}

}
