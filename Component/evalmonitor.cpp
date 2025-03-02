#include "Component/Monitor.hpp"
#include "NetworkTopology.h"
#include "ParameterContainer.h"
#include "modular_components.hpp"
#include "utils.h"

namespace MiniDNN {

// Generic definition for computing accuracy, shared by other monitors

double Monitor::eval_accuracy(bool training_set) {
    auto *netw = new NetworkTopology(*_exec.get_model()->get_network());

    const auto t1 = HRClock::now();

    // std::cout << "Forward pass over training set...\n";
    netw->forward(training_set 
                  ? this->_exec.get_batcher()->_train_x 
                  : this->_exec.get_batcher()->_test_x);

    const Matrix &preds = netw->get_last_layer()->output();
    double accur = compute_accuracy(preds, training_set 
                                    ? this->_exec.get_batcher()->_train_y 
                                    : this->_exec.get_batcher()->_test_y);

    delete netw;
    const auto t2 = HRClock::now();
    std::cout << "Evaluating accuracy took " << std::chrono::duration<double>(t2 - t1).count() << " seconds \n";
    return accur;
}

void Monitor::background_submit_accuracy(int epoch_nr) {
    _qmtx.lock();
    _netws_to_eval.emplace_back(epoch_nr, new NetworkTopology(*_exec.get_model()->get_network()));
    _qmtx.unlock();
}

void Monitor::_thread_submit_accuracy(int cpu) {
    // std::cout << "Accuracy worker: running.\n";
    set_cpu(cpu);
    while (true) {
        if (_exec.get_dispatcher()->is_finished() && _netws_to_eval.empty()) break;
        _qmtx.lock();
        if (_netws_to_eval.empty()) {
            _qmtx.unlock();
            continue;
        } else {
            // const auto t1 = HRClock::now();
            auto [epoch_nr, netw] = std::move(_netws_to_eval.front());
            _netws_to_eval.pop_front();
            _qmtx.unlock();

            netw->forward(this->_exec.get_batcher()->_test_x);

            const Matrix &preds = netw->get_last_layer()->output();
            const double accur = compute_accuracy(preds, this->_exec.get_batcher()->_test_y);
            delete netw;

            // const auto t2 = HRClock::now();

            // std::cout << "[accur " << cpu << "] Computed accuracy. Queue sz = " << _netws_to_eval.size() << "\n";
            // std::cout << "[accur " << cpu << "] Took " << std::chrono::duration<double>(t2 - t1).count() << " secs\n";
            _accmtx.lock();
            this->epoch_accuracies[epoch_nr] = accur;
            _accmtx.unlock();
        }
    }
}

EvalMonitor::EvalMonitor(SystemExecutor &exec, double alpha, long eval_interval,
                         int eval_batch_size, bool use_mtx)
    : Monitor(exec), use_mtx(use_mtx), local_batcher(exec, "CIFAR10", eval_batch_size),
    alpha(alpha), eval_interval(eval_interval) {
}

void EvalMonitor::update(double loss, long duration_ns, long step) {
    if (_exec.get_dispatcher()->is_finished()) return;
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
    this->_exec.get_paracontr()->update(step);

    if (step > 0 && step % _exec._steps_per_epoch == 0) {
        const double avg_loss = this->get_loss_accur();

        std::cout << "[monitor] Completed epoch " << step / _exec._steps_per_epoch
                  << ". Evaluated Loss = " << avg_loss
                  << ". EMA Loss = " << ema_loss << std::endl;
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

double EvalMonitor::get_loss_estim() {
    return ema_loss;
}

double EvalMonitor::get_rate_estim() {
    return ema_rate;
}

double EvalMonitor::get_loss_accur() {
    auto* network = new NetworkTopology(*_exec.get_model()->get_network());
    int batch_sz;
    const auto bid = this->local_batcher.get_batch_ind(-1, std::make_unique<int>(batch_sz));
    const Matrix &b_x = this->local_batcher.get_batch_data(bid, batch_sz);
    const Matrix &b_y = this->local_batcher.get_batch_labels(bid, batch_sz);

    network->forward(b_x);

    // Evaluate how well the last layer outputs match the labels.
    network->get_output()->check_target_data(b_y);
    network->get_output()->evaluate(network->get_last_layer()->output(), b_y);

    const double loss = network->get_loss();

    delete network;
    return loss;
}

}
