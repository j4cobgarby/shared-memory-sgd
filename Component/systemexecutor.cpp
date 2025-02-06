#include "modular_components.hpp"

long MiniDNN::SystemExecutor::elapsed_time() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()
        .time_since_epoch()
    ).count() - this->start_time_ms;
}

void MiniDNN::SystemExecutor::start() {
    this->workers->start_all();
    this->workers->wait_for_all();
}

long MiniDNN::SystemExecutor::submit_para_change(long m, bool is_probing) {
    this->mtx_para_vec.lock();
    this->_para_mstimes.push_back(this->elapsed_time());
    this->_para_values.push_back(m);
    this->para_is_probing.push_back(is_probing);
    this->_para_stepinds.push_back(dispatcher->get_steps_done());
    this->mtx_para_vec.unlock();
    
    return m;
}

void MiniDNN::SystemExecutor::submit_steptimes(std::vector<std::tuple<long, long, long, long>>& ts) {
    this->mtx_steptime_samples.lock();
    this->_steptime_samples.insert(this->_steptime_samples.end(), ts.begin(), ts.end());
    this->mtx_steptime_samples.unlock();
}

void MiniDNN::SystemExecutor::submit_tau_dist(const std::array<long, MAX_TAU_DIST> &new_dist) {
    _mtx_tau_dist.lock();
    for (int tau = 0; tau < MAX_TAU_DIST; tau++) {
        _tau_dist.at(tau) += new_dist.at(tau);
    }
    _mtx_tau_dist.unlock();
}

void MiniDNN::SystemExecutor::submit_epoch_tau_dist(const std::vector<std::array<long, MAX_MEASURE_TAU_PER_EPOCH>> &ds) {
    _mtx_epoch_tau_dist.lock();
    if (ds.size() > _epoch_tau_dist.size()) {
        _epoch_tau_dist.resize(ds.size());
    }

    for (size_t ep = 0; ep < ds.size(); ep++) {
        for (int tau = 0; tau < MAX_MEASURE_TAU_PER_EPOCH; tau++) {
            _epoch_tau_dist.at(ep).at(tau) += ds.at(ep).at(tau);
        }
    }
    _mtx_epoch_tau_dist.unlock();
}

void MiniDNN::SystemExecutor::submit_acceptance_rate(const long accepted, const long rejected) {
    _mtx_accept_rate.lock();
    _accepted_steps += accepted;
    _rejected_steps += rejected;
    _mtx_accept_rate.unlock();
}

void MiniDNN::SystemExecutor::submit_thread_epochs(const std::vector<double> L, const std::vector<int> N) {
    mtx_thread_epoch_sums.lock();
    if (_thread_epoch_sums.size() < L.size()) {
        _thread_epoch_sums.resize(L.size());
        _thread_epoch_counts.resize(L.size());
    }

    for (int i = 0; i < L.size(); i++) {
        _thread_epoch_sums.at(i) += L.at(i);
        _thread_epoch_counts.at(i) += N.at(i);
    }
    mtx_thread_epoch_sums.unlock();
}
