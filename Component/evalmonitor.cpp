#include "Component/Monitor.hpp"
#include "NetworkTopology.h"
#include "ParameterContainer.h"
#include "modular_components.hpp"

namespace MiniDNN {

EvalMonitor::EvalMonitor(SystemExecutor &exec, double alpha, long eval_interval,
                         int eval_batch_size, bool use_mtx)
    : Monitor(exec), use_mtx(use_mtx), local_batcher(exec, "CIFAR10", eval_batch_size),
    eval_interval(eval_interval) {
}

void EvalMonitor::update(double loss) {
    if (use_mtx) mtx.lock();

    if (!got_initial) {
        got_initial = true;
        ema = loss;
    } else {
        ema = alpha * loss + (1.0 - alpha) * ema;
    }

    if (use_mtx) mtx.unlock();
}

double EvalMonitor::get_loss_estim() {
    return ema;
}

double EvalMonitor::get_loss_accur() {
    ParameterContainer *global_param_ptr;
    NetworkTopology *network;

    global_param_ptr = exec.get_model()->get_network()->current_param_container_ptr;
    network = new NetworkTopology(*exec.get_model()->get_network());
    network->set_pointer(global_param_ptr);

    int bid = this->local_batcher.get_batch_ind(-1);
    const Matrix &b_x = this->local_batcher.get_batch_data(bid);
    const Matrix &b_y = this->local_batcher.get_batch_labels(bid);

    network->forward(b_x);

    // Evaluate how well the last layer outputs match the labels.
    network->get_output()->check_target_data(b_y);
    network->get_output()->evaluate(network->get_last_layer()->output(), b_y);

    return network->get_loss();
}

}
