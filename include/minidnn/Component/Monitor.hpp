#ifndef COMPONENT_MONITOR_HPP
#define COMPONENT_MONITOR_HPP

#include "Component/BatchController.hpp"
#include "modular_components.hpp"
#include <limits>

namespace MiniDNN {

class SlidingWindowMonitor : public Monitor {
private:
    int window_size;

    // Window of measurements.
    // Tuple:
    //  0: Absolute loss reported after SGD step
    //  1: Delta loss between this and previous reported step
    //  2: Time taken to complete this step
    std::vector<std::tuple<double, double, long>> window;
    double last_reported_loss = -1;

    std::mutex mtx;
public:
    SlidingWindowMonitor(SystemExecutor &exec, int window_size);

    void update(double loss, long duration_ns, long step) override;
    double get_loss_estim() override;
    double get_rate_estim() override;
    double get_loss_accur() override { return get_loss_estim(); }
};

class EMAMonitor : public Monitor {
private:
    double alpha;

    double last_reported_loss = -1;
    double ema_loss = std::numeric_limits<double>::infinity(); // More or less arbitrary
    double ema_rate = std::numeric_limits<double>::infinity(); // More or less arbitrary

    bool got_initial = false;
    bool use_mtx;
    std::mutex mtx;
public:
    EMAMonitor(SystemExecutor &exec, double alpha, bool use_mtx);

    void update(double loss, long duration_ns, long step) override;
    double get_loss_estim() override;
    double get_rate_estim() override;
    double get_loss_accur() override { return get_loss_estim(); }
};

class EvalMonitor : public Monitor {
private:
    bool use_mtx;
    std::mutex update_mtx;

    /* Values used for EMA calculation (for the low-accuracy estimations) */
    double alpha;
    double last_reported_loss = -1;
    double ema_loss = std::numeric_limits<double>::infinity();
    double ema_rate = std::numeric_limits<double>::infinity();
    bool got_initial = false;

    long eval_interval;

    /* Local instance of a batch controller so that it can get batches of data
     * big enough for accurate evaluation */
    SimpleBatchController local_batcher;
public:
    EvalMonitor(SystemExecutor &exec, double alpha, long eval_interval,
                int eval_batch_size, bool use_mtx);

    void update(double loss, long duration_ns, long step) override;
    double get_loss_estim() override;
    double get_rate_estim() override;
    /* Properly calculate the training loss */
    double get_loss_accur() override;
};

}

#endif /* COMPONENT_MONITOR_HPP */
