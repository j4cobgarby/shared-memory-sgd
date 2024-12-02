#include "Component/Monitor.hpp"
#include "modular_components.hpp"

namespace MiniDNN {

EMAMonitor::EMAMonitor(SystemExecutor &exec, double alpha, bool use_mtx)
    : Monitor(exec), alpha(alpha), use_mtx(use_mtx) { 
}

void EMAMonitor::update(double loss, long duration_ns, long step) {
    double rate = last_reported_loss >= 0 ? loss - last_reported_loss : 0.0;
    rate /= static_cast<double>(duration_ns) / 1e9;

    if (!got_initial) {
        got_initial = true;
        ema_loss = loss;
        ema_rate = 0.0;
    } else {
        ema_loss = alpha * loss + (1.0 - alpha) * ema_loss;
        ema_rate = alpha * rate + (1.0 - alpha) * ema_rate;
    }

    /* Allow the parallelism controller to update now */
    if (use_mtx) mtx.lock();
    this->exec.get_paracontr()->update(step);
    if (use_mtx) mtx.unlock();

    if (step > 0 && step % exec.steps_per_epoch == 0) {
        const double avg_loss = this->get_loss_estim();

        std::cout << "[monitor] Completed epoch " << step / exec.steps_per_epoch
                  << ". EMA Loss = " << avg_loss << std::endl;

        exec.mtx_epoch_vec.lock();
        exec.epoch_losses.push_back(avg_loss);
        exec.epoch_mstimes.push_back(exec.elapsed_time());
        exec.mtx_epoch_vec.unlock();
    }
}

double EMAMonitor::get_loss_estim() {
    return ema_loss;
}

double EMAMonitor::get_rate_estim() {
    return ema_rate;
}


}
