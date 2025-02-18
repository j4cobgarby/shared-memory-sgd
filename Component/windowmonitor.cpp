#include "Component/Monitor.hpp"
#include "modular_components.hpp"

#include <numeric>

namespace MiniDNN {

SlidingWindowMonitor::SlidingWindowMonitor(SystemExecutor &exec, const int window_size) : Monitor(exec),
                                                                                          window_size(window_size) {
    window.resize(window_size);

    std::cout << "Starting accuracy computation thread.\n";
}

void SlidingWindowMonitor::update(double loss, long duration_ns, long step) {
    unsigned long this_ins = ins_count.fetch_add(1) % window_size;
    // mtx.lock(); // Wide mutex here, because otherwise segfault can happen with concurrent vector modification

    this->window.at(this_ins) = {
        loss,
        last_reported_loss >= 0 ? loss - last_reported_loss : 0.0,
        duration_ns
    };

    // next_window_ins = (next_window_ins + 1) % window.size();
    window_filled++;
    if (window_filled > window_size) window_filled = window_size;

    /* Allow the parallelism controller to update now */
    this->_exec.get_paracontr()->update(step);

    if (step > 0 && step % _exec._steps_per_epoch == 0) {
        const double avg_loss = this->get_loss_estim();

        // std::cout << "[monitor] Completed epoch " << step / _exec._steps_per_epoch
                  // << ". Loss = " << avg_loss << std::endl;
        std::cout << "-> Submitted network for accuracy\n";
        background_submit_accuracy();

        if (step / _exec._steps_per_epoch == 1) {
            _exec.first_loss = avg_loss;
            _exec.got_first_loss = true;
        }

        _exec.mtx_epoch_vec.lock();
        _exec._epoch_losses.push_back(avg_loss);
        _exec._epoch_mstimes.push_back(_exec.elapsed_time());
        _exec.mtx_epoch_vec.unlock();
    }
}

double SlidingWindowMonitor::get_loss_estim() {
    double sum = 0;
    for (size_t i = 0; i < window_filled; i++) {
        const auto [loss, delta, dur] = window.at(i);
        sum += loss;
    }
    return sum / static_cast<double>(window_filled);
}

double SlidingWindowMonitor::get_rate_estim() {
    double rate = 0;
    for (size_t i = 0; i < window_filled; i++) {
        const auto [loss, delta, dur] = window.at(i);
        rate += delta / (static_cast<double>(dur) / 1e9);
    }
    return rate / window_filled;
}


}
