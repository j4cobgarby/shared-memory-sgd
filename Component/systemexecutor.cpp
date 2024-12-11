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
    this->para_mstimes.push_back(this->elapsed_time());
    this->para_values.push_back(m);
    this->para_is_probing.push_back(is_probing);
    this->mtx_para_vec.unlock();
    
    return m;
}

void MiniDNN::SystemExecutor::submit_steptimes(std::vector<std::tuple<long, long, long>>& ts) {
    this->mtx_steptime_samples.lock();
    std::cout << "[exec] Thread submitting " << ts.size() << " step times." << std::endl;
    this->steptime_samples.insert(this->steptime_samples.end(), ts.begin(), ts.end());
    this->mtx_steptime_samples.unlock();
}
