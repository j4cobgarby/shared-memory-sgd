#include "Component/Monitor.hpp"
#include "modular_components.hpp"

namespace MiniDNN {

EMAMonitor::EMAMonitor(SystemExecutor &exec, double alpha, bool use_mtx)
    : Monitor(exec), alpha(alpha), use_mtx(use_mtx) { 
}

void EMAMonitor::update(double loss) {
    if (use_mtx) mtx.lock();

    if (!got_initial) {
        got_initial = true;
        ema = loss;
    } else {
        ema = alpha * loss + (1.0 - alpha) * ema;
    }

    mtx.unlock();
}

double EMAMonitor::get_loss() {
    return ema;
}

}
