#ifndef COMPONENT_MONITOR_HPP
#define COMPONENT_MONITOR_HPP

#include "modular_components.hpp"

namespace MiniDNN {

class SlidingWindowMonitor : public Monitor {
private:
    int window_size;
    std::vector<double> window;
public:
    SlidingWindowMonitor(SystemExecutor &exec, int window_size);

    void update(double loss) override;
    double get_loss() override;
};

}

#endif /* COMPONENT_MONITOR_HPP */
