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
    this->_exec.get_paracontr()->update(step);
    if (use_mtx) mtx.unlock();

    if (step > 0 && step % _exec._steps_per_epoch == 0) {
        const double avg_loss = this->get_loss_estim();

        std::cout << "[monitor] Completed epoch " << step / _exec._steps_per_epoch
                  << ". EMA Loss = " << avg_loss << std::endl;

        _exec.mtx_epoch_vec.lock();
        _exec._epoch_losses.push_back(avg_loss);
        _exec._epoch_mstimes.push_back(_exec.elapsed_time());
        _exec._epoch_accur.push_back(_exec.get_monitor()->eval_accuracy());
        _exec.mtx_epoch_vec.unlock();
    }
}

double EMAMonitor::get_loss_estim() {
    return ema_loss;
}

double EMAMonitor::get_rate_estim() {
    return ema_rate;
}


}
