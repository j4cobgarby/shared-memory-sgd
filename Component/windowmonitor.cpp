#include "Component/Monitor.hpp"
#include "modular_components.hpp"

#include <numeric>

namespace MiniDNN {

SlidingWindowMonitor::SlidingWindowMonitor(SystemExecutor &exec, int window_size) : Monitor(exec), window_size(window_size) { }

void SlidingWindowMonitor::update(double loss) {
    static long u = 0;

    this->window.push_back(loss);
    
    /* Pop first element if the window is full */
    if (this->window.size() > this->window_size) {
        this->window.erase(this->window.begin());
    }

    /* Allow the parallelism controller to update now */
    this->exec.get_paracontr()->update();

    // if (u++ % 64 == 0) {
    //     std::cout << "[monitor] Update " << u << ". Window avg = " << this->get_loss() << std::endl;
    // }
}

double SlidingWindowMonitor::get_loss() {
    return std::reduce(this->window.begin(), this->window.end()) / static_cast<double>(this->window.size());
}

}
