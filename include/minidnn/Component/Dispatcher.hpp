#ifndef COMPONENT_DISPATCHER_HPP
#define COMPONENT_DISPATCHER_HPP

#include "modular_components.hpp"
#include <atomic>
#include <cmath>
#include <limits>

namespace MiniDNN {

class AsyncDispatcher : public Dispatcher {
public:
    AsyncDispatcher(SystemExecutor &exec) : Dispatcher(exec) {}

    std::pair<bool, long> try_start_step(long worker_id) override;
    bool finish_step(long worker_id, long step_ind, long &end_step_ind) override;
    bool is_finished() override;
};

class SemiSyncDispatcher : public Dispatcher {
private:
    long async_period; /* Number of steps to complete before synchronising */
    long async_period_min;

    int period_update_counter = 0;
    int steps_until_period_update;

    int period_reduce_step;

    int win_phase_counter = 0; // Cycles in [0...up+down+1]; last value means execution
    int win_probe_period, win_exec_period;
    int win_up = 1, win_down = 3;
    int win_max_log = 12, win_min_log = 1;
    int win_log_of_period; // We actually probe powers of two, so this is what we adjust directly
    int win_best_log = -1;
    double win_best_rate = std::numeric_limits<double>::infinity();

    std::atomic<long> period_start_step = 0;
    std::atomic<long> steps_done_in_period = 0;


    void update_period() {
        switch (strat) {
        case YUPDATE_DECAY:
            _period_decay();
            break;
        case YUPDATE_PROBE:
            _window_probe();
            break;
        }
    }
    void _window_probe();
    void _period_decay();
public:
    enum update_strat { YUPDATE_DECAY, YUPDATE_PROBE };

    SemiSyncDispatcher(SystemExecutor &exec,
                       update_strat strat, long P_0,
                       long P_min, int reduce_period, int reduce_step, // Decay params
                       int win_probe_period, int win_exec_period) :
        Dispatcher(exec), strat(strat),
        async_period(P_0), async_period_min(P_min),
        period_reduce_step(reduce_step),
        win_probe_period(win_probe_period), win_exec_period(win_exec_period) {

        switch (strat) {
        case YUPDATE_DECAY:
            steps_until_period_update = reduce_period;
            break;
        case YUPDATE_PROBE:
            std::cout << "period until update = " << win_probe_period << "\n";
            steps_until_period_update = win_probe_period;
            win_log_of_period = int(log2(P_0));
            async_period = 1 << win_log_of_period;
            break;
        }
    }

    std::pair<bool, long> try_start_step(long worker_id) override;
    bool finish_step(long worker_id, long step_ind, long &end_step_ind) override;
    bool is_finished() override;

private:
    update_strat strat;
};

class ResettingDispatcher : public Dispatcher {
public:
    explicit ResettingDispatcher(SystemExecutor& exec)
        : Dispatcher(exec)
    {
    }

    std::pair<bool, long> try_start_step(long worker_id) override;
    bool finish_step(long worker_id, long step_ind, long &end_step_ind) override;
    bool is_finished() override;
};

}

#endif /* COMPONENT_DISPATCHER_HPP */
