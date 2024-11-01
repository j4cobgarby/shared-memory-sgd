#include "Component/Monitor.hpp"
#include "modular_components.hpp"

#include <numeric>

namespace MiniDNN {

SlidingWindowMonitor::SlidingWindowMonitor(SystemExecutor &exec, int window_size) : Monitor(exec), window_size(window_size) {

}

void SlidingWindowMonitor::update(double loss) {
    const long s = exec.get_dispatcher()->get_steps_done();

    this->window.push_back(loss);

    /* Pop first element if the window is full */
    if (this->window.size() > this->window_size) {
        this->window.erase(this->window.begin());
    }

    /* Allow the parallelism controller to update now */
    this->exec.get_paracontr()->update();

    if (s % exec.steps_per_epoch == 0) {
        const double avg_loss = this->get_loss();

        std::cout << "[monitor] Completed epoch " << s / exec.steps_per_epoch
                  << ". Loss = " << avg_loss << std::endl;

        exec.epoch_losses.push_back(avg_loss);
        exec.epoch_mstimes.push_back(exec.elapsed_time());
    }
}

double SlidingWindowMonitor::get_loss() {
    return std::reduce(this->window.begin(), this->window.end()) / static_cast<double>(this->window.size());
}

}
