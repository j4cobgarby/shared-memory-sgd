#include "Component/Dispatcher.hpp"
#include "utils.h"
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
    
    const double loss_now = _exec.get_monitor()->get_loss_accur();
    
    /* First: Based on phase in probing cycle, update log(period) */
    if (win_phase_counter < 1+(win_up + win_down)/win_step) { // Just finished a probe
        std::cout << "Finished probe at y = " << async_period << "\n";
        const double phase_dur = (double)(HRClock::now() - win_phase_start_time).count() * 1e-9;
        const double this_rate = win_phase_start_loss >= 0
            ? (loss_now - win_phase_start_loss) / phase_dur
            : 0.0; // No change if this was the first ever phase

        if (this_rate < win_best_rate) {
            win_best_rate = this_rate;
            win_best_period = async_period;
            std::cout << "New best rate: y =" << win_best_period << ", rate = " << win_best_rate << "\n";
        }

        win_phase_counter++;

        if (win_phase_counter == 1+(win_up + win_down)/win_step) { // Start exec
            async_period = win_best_period;
            steps_until_period_update = win_exec_period;
            std::cout << "Exec at y =" << async_period << "\n";
        }
        else { // Start next probe
            async_period -= win_step;
            if (async_period < async_period_min) {
                async_period = async_period_min;
            }
        }
    }
    else if (win_phase_counter == 1+(win_up + win_down)/win_step) // Just finished execution
    {
        const double loss_perc = _exec.got_first_loss ? loss_now / _exec.first_loss : 1.0;
        const double scale_factor = loss_perc * win_loss_scalar + (1 - win_loss_scalar);

        std::cout << "Finished exec at y = " << async_period << "\n";
        std::cout << "Loss % = " << loss_perc << ", scale factor = " << scale_factor << "\n";
        win_phase_counter = 0;
        async_period += win_up * scale_factor; // Start probing from top
        win_step = (int)std::round((float)win_step_base * (float)scale_factor);
        win_best_rate = std::numeric_limits<double>::infinity();
        steps_until_period_update = win_probe_period;
        std::cout << "Completed exec. New step = " << win_step << "\n";
    }
    else {
        std::cerr << "[SemiSyncDispatcher] Bug: We seem to have reached an undefined probing phase number\n";
    }

    this->_exec.mtx_async_period_vec.lock();
    this->_exec._async_period_mstimes.push_back(this->_exec.elapsed_time());
    this->_exec._async_period_values.push_back(async_period);
    this->_exec.mtx_async_period_vec.unlock();

    /* Finally: Record the loss for the beginning of this next phase */
    win_phase_start_loss = loss_now;
    win_phase_start_time = HRClock::now();
}

bool SemiSyncDispatcher::is_finished() {
    return this->_exec.elapsed_time() >= 1000 * 300;
    // return this->_steps_done >= _exec._epoch_target * _exec._steps_per_epoch;
}

}
