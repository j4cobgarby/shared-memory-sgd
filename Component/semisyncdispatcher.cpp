#include "Component/Dispatcher.hpp"
#include <atomic>
#include <limits>

namespace MiniDNN {

std::pair<bool, long> SemiSyncDispatcher::try_start_step(long worker_id) {
    if (this->is_finished())
        return {false, 0};

    // TODO: We can perform a big optimisation here by preventing threads starting if we have completed a period and are waiting.
    return {worker_id < this->_exec.get_paracontr()->get_parallelism(), _steps_started.fetch_add(1)};

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
        // int c = period_update_counter.fetch_add(async_period, std::memory_order::relaxed);
        period_update_counter += async_period;

        if (period_update_counter > steps_until_period_update) {
            period_update_counter = 0;

            update_period();
        }

        // period_update_counter += async_period;
        // Start a new period

        // Reduce period size by 1 if we completed 8192 steps in this period so far
        // if (period_update_counter > steps_until_period_update) {
        //     period_update_counter = 0;
        //     async_period -= period_reduce_step;
        //     if (async_period < async_period_min) {
        //         async_period = async_period_min;
        //     }
        //
        //     this->_exec.mtx_async_period_vec.lock();
        //     this->_exec._async_period_mstimes.push_back(this->_exec.elapsed_time());
        //     this->_exec._async_period_values.push_back(async_period);
        //     this->_exec.mtx_async_period_vec.unlock();
        // }

        period_start_step.store(_steps_started, std::memory_order::release);
        steps_done_in_period.store(0, std::memory_order::release);
    }

    end_step_ind = this->_steps_done.fetch_add(1);

    return true;
}

void SemiSyncDispatcher::_period_decay() {
    async_period -= period_reduce_step;
    if (async_period < async_period_min) {
        async_period = async_period_min;
    }

    this->_exec.mtx_async_period_vec.lock();
    this->_exec._async_period_mstimes.push_back(this->_exec.elapsed_time());
    this->_exec._async_period_values.push_back(async_period);
    this->_exec.mtx_async_period_vec.unlock();
}

void SemiSyncDispatcher::_window_probe() {
    // This is called when the last "update interval" ended, so we have to either start the next probe,
    // or begin execution.
    
    /* First: Based on phase in probing cycle, update log(period) */
    if (win_phase_counter < win_up + win_down + 1) { // Just finished a probe
        double this_rate = _exec.get_monitor()->get_rate_estim();
        std::cout << "Completed probe at " << 1 << win_log_of_period << " => " << this_rate << "\n";

        if (this_rate < win_best_rate) {
            win_best_rate = this_rate;
            win_best_log = win_log_of_period;
            std::cout << "New best rate: log(y)=" << win_best_log << ", rate = " << win_best_rate << "\n";
        }

        win_phase_counter++;

        if (win_phase_counter == win_up + win_down + 1) { // Start exec
            win_log_of_period = win_best_log;
            steps_until_period_update = win_exec_period;
            std::cout << "Exec at log(y)=" << win_best_log << "\n";
        }
        else { // Start next probe
            win_log_of_period--;
        }
    }
    else if (win_phase_counter == win_up + win_down + 1) // Just finished execution
    {
        win_phase_counter = 0;
        win_log_of_period += win_up; // Start probing from top
        win_best_rate = std::numeric_limits<double>::infinity();
        steps_until_period_update = win_probe_period;
        std::cout << "Completed exec\n";
    }
    else {
        std::cerr << "[SemiSyncDispatcher] Bug: We seem to have reached an undefined probing phase number\n";
    }

    /* Second: Set actual period based on log, and report */
    async_period = 1 << win_log_of_period;

    this->_exec.mtx_async_period_vec.lock();
    this->_exec._async_period_mstimes.push_back(this->_exec.elapsed_time());
    this->_exec._async_period_values.push_back(async_period);
    this->_exec.mtx_async_period_vec.unlock();
}

bool SemiSyncDispatcher::is_finished() {
    return this->_exec.elapsed_time() >= 1000 * 600;
    // return this->_steps_done >= _exec._epoch_target * _exec._steps_per_epoch;
}

}
