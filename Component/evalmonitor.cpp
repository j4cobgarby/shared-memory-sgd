#include "Component/Monitor.hpp"
#include "NetworkTopology.h"
#include "ParameterContainer.h"
#include "modular_components.hpp"
#include <type_traits>

namespace MiniDNN {

EvalMonitor::EvalMonitor(SystemExecutor &exec, long eval_interval, int eval_batch_size, bool use_mtx)
    : Monitor(exec), use_mtx(use_mtx), local_batcher(exec, "CIFAR10", eval_batch_size),
    eval_interval(eval_interval) {
}

void EvalMonitor::update(double loss) {
}

double EvalMonitor::get_loss() {
    return last_eval_loss;
}

/* This function is run as a background thread.
 * It continuously takes the newest version of the global model and evaluates
 * it against large (relative to the training batches) data batches, but still
 * taken from the training data. TODO: Should this actually be from the evaluation
 * data? */
void EvalMonitor::eval_thread() {
    // This holds a pointer to the global parameters. Pointer gets populated after
    // the first time this thread starts an evaluation.
    ParameterContainer *global_param_ptr = nullptr;

    NetworkTopology *network = nullptr;

    long last_eval_step = 0;

    // Thread should finish once execution is done
    while (!exec.get_dispatcher()->is_finished()) {
        long steps_done = exec.get_dispatcher()->get_steps_done();
        if (steps_done >= last_eval_step + eval_interval) {
            last_eval_step = steps_done;

            if (!global_param_ptr) {
                global_param_ptr = exec.get_model()->get_network()->current_param_container_ptr;
                network = new NetworkTopology(*exec.get_model()->get_network());
                network->set_pointer(global_param_ptr);
            }

            int bid = this->local_batcher.get_batch_ind(-1);
            const Matrix &b_x = this->local_batcher.get_batch_data(bid);
            const Matrix &b_y = this->local_batcher.get_batch_labels(bid);

            network->forward(b_x);

            // Evaluate how well the last layer outputs match the labels.
            network->get_output()->check_target_data(b_y);
            network->get_output()->evaluate(network->get_last_layer()->output(), b_y);

            this->last_eval_loss = network->get_loss();
        }
    }
}

}
