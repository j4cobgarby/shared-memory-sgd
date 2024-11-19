#ifndef COMPONENT_MONITOR_HPP
#define COMPONENT_MONITOR_HPP

#include "Component/BatchController.hpp"
#include "modular_components.hpp"
#include <limits>

namespace MiniDNN {

class SlidingWindowMonitor : public Monitor {
private:
    int window_size;
    std::vector<double> window;
    std::mutex mtx;
public:
    SlidingWindowMonitor(SystemExecutor &exec, int window_size);

    void update(double loss) override;
    double get_loss() override;
};

class EMAMonitor : public Monitor {
private:
    double alpha;
    double ema = std::numeric_limits<double>::infinity(); // More or less arbitrary
    bool got_initial = false;
    bool use_mtx;
    std::mutex mtx;
public:
    EMAMonitor(SystemExecutor &exec, double alpha, bool use_mtx);

    void update(double loss) override;
    double get_loss() override;
};

class EvalMonitor : public Monitor {
private:
    double last_eval_loss;
    bool use_mtx;
    std::mutex mtx;

    long eval_interval;

    /* Local instance of a batch controller so that it can get batches of data
     * big enough for accurate evaluation */
    SimpleBatchController local_batcher;

    void eval_thread();
public:
    EvalMonitor(SystemExecutor &exec, long eval_interval, int eval_batch_size, bool use_mtx);

    void update(double loss) override;
    double get_loss() override;
};

}

#endif /* COMPONENT_MONITOR_HPP */
