#include "modular_components.hpp"

long MiniDNN::SystemExecutor::elapsed_time() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()
        .time_since_epoch()
    ).count() - this->start_time_ms;
}

void MiniDNN::SystemExecutor::start() {
    this->start_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()
        .time_since_epoch()
    ).count();

    this->workers->start_all();
    this->workers->wait_for_all();
}
