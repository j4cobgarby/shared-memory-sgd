#include "Component/Monitor.hpp"
#include "modular_components.hpp"

#include <numeric>

namespace MiniDNN {

SlidingWindowMonitor::SlidingWindowMonitor(SystemExecutor &exec, const int window_size) : Monitor(exec),
                                                                                          window_size(window_size) {
    window.reserve(window_size);
}

void SlidingWindowMonitor::update(double loss, long duration_ns) {
    mtx.lock();
    const long s = exec.get_dispatcher()->get_steps_done();
    // std::cout << "[monitor] Finished " << s << " steps.\n";

    this->window.emplace_back(
        loss,
        last_reported_loss >= 0 ? loss - last_reported_loss : 0.0,
        duration_ns
    );

    /* Pop first element if the window is full */
    if (this->window.size() > this->window_size) {
        this->window.erase(this->window.begin());
    }

    /* Allow the parallelism controller to update now */
    this->exec.get_paracontr()->update();
    mtx.unlock();

    if (s % exec.steps_per_epoch == 0) {
        const double avg_loss = this->get_loss_estim();

        std::cout << "[monitor] Completed epoch " << s / exec.steps_per_epoch
                  << ". Loss = " << avg_loss << std::endl;

        exec.mtx_epoch_vec.lock();
        exec.epoch_losses.push_back(avg_loss);
        exec.epoch_mstimes.push_back(exec.elapsed_time());
        exec.mtx_epoch_vec.unlock();
    }
}

double SlidingWindowMonitor::get_loss_estim() {
    double sum = 0;
    for (const auto [loss, delta, dur] : window) {
        sum += loss;
    }
    return sum / static_cast<double>(this->window.size());
}

double SlidingWindowMonitor::get_rate_estim() {
    double rate = 0;
    for (const auto [loss, delta, dur] : window) {
        rate += delta / (static_cast<double>(dur) / 1e9);
    }
    return rate / static_cast<double>(this->window.size());
}


}
