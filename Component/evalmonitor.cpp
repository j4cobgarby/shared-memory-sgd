#include "Component/Monitor.hpp"
#include "NetworkTopology.h"
#include "ParameterContainer.h"
#include "modular_components.hpp"

namespace MiniDNN {

EvalMonitor::EvalMonitor(SystemExecutor &exec, double alpha, long eval_interval,
                         int eval_batch_size, bool use_mtx)
    : Monitor(exec), use_mtx(use_mtx), local_batcher(exec, "CIFAR10", eval_batch_size),
    alpha(alpha), eval_interval(eval_interval) {
}

void EvalMonitor::update(double loss, long duration_ns) {
    if (exec.get_dispatcher()->is_finished()) return;
    if (use_mtx) update_mtx.lock();
    const long s = exec.get_dispatcher()->get_steps_done();
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
    this->exec.get_paracontr()->update();
    if (use_mtx) update_mtx.unlock();

    if (s % exec.steps_per_epoch == 0) {
        const double avg_loss = this->get_loss_accur();

        std::cout << "[monitor] Completed epoch " << s / exec.steps_per_epoch
                  << ". Evaluated Loss = " << avg_loss
                  << ". EMA Loss = " << ema_loss << std::endl;

        exec.mtx_epoch_vec.lock();
        exec.epoch_losses.push_back(avg_loss);
        exec.epoch_mstimes.push_back(exec.elapsed_time());
        exec.mtx_epoch_vec.unlock();
    }
}

double EvalMonitor::get_loss_estim() {
    return ema_loss;
}

double EvalMonitor::get_rate_estim() {
    return ema_rate;
}

double EvalMonitor::get_loss_accur() {

    ParameterContainer* global_param_ptr = exec.get_model()->get_network()->current_param_container_ptr;
    auto* network = new NetworkTopology(*exec.get_model()->get_network());
    network->set_pointer(global_param_ptr);

    const int bid = this->local_batcher.get_batch_ind(-1);
    const Matrix &b_x = this->local_batcher.get_batch_data(bid);
    const Matrix &b_y = this->local_batcher.get_batch_labels(bid);

    network->forward(b_x);

    // Evaluate how well the last layer outputs match the labels.
    network->get_output()->check_target_data(b_y);
    network->get_output()->evaluate(network->get_last_layer()->output(), b_y);

    const double loss = network->get_loss();

    delete network;
    return loss;
}

}
