#pragma once

#include "modular_components.hpp"
#include <chrono>
#include <vector>

namespace MiniDNN {

/* A simple parallelism controller which maintains a static parallelism at all times */
class StaticParaController : public ParaController {
private:
    unsigned m;
public:
    StaticParaController(SystemExecutor &exec, unsigned m) : ParaController(exec), m(m) {}
    unsigned get_parallelism() override { return m; }
    void update() override {}
};

class WindowParaController : public ParaController {
private:
    /* Probing logic */
    bool is_probing = true;
    unsigned curr_parallelism;
    long phase_start_step = -1;
    int window_btm;
    unsigned best_probe_m;
    double best_convrate;

    /* Used for calculating delta loss */
    std::chrono::time_point<HRClock> t_stage_start;
    double loss_start_of_stage;

    const long probe_steps, exec_steps;
    const int window_size;
    const int total_workers;

    void switch_to_para(unsigned m);
    void clip_window();
public:
    WindowParaController(SystemExecutor &exec, int num_threads, int window_size, long probe_steps, long exec_steps);

    unsigned get_parallelism() override;
    void update() override;
};

class SearchParaController : public ParaController {
private:
    bool is_searching;

    // Searching Variables
    // Probe num within current search, set to 0 when searching starts.
    // Probe 0 is first probe.
    // This means that there are 3*search_degree probes per search.
    int probe_counter = 0;

    // How "deep" is the search? i.e. how many times is the search space split?
    const int search_degree;

    const int total_workers;

    const int window_size;

    int low_bound, high_bound;

    // Which number is the first step in the current phase?
    long phase_start_step = -1;

    long probe_steps, exec_steps;

    int best_probe = 0; // in {0,1,2}
    unsigned best_probe_m = 0;
    double best_probe_loss = std::numeric_limits<double>::infinity();

    unsigned curr_parallelism = 0;

    void shrink_bounds();
    void switch_to_para(unsigned m);
    void clip_window();
public:
    SearchParaController(SystemExecutor &exec, int num_threads, int search_degree,
        long probe_steps, long exec_steps, int window_size);

    unsigned get_parallelism() override;

    void update() override;
};

// TODO: A ParaController which replays some recorded data of parallelism generated
// by a different ParaController, with the intention of allowing averaging of runs
class ReplayParaController : public ParaController {

};

class PatternController : public ParaController {
public:
    enum pattern_type {
        STATIC,
        RAMP,
    };

    struct pattern_element {
        pattern_type type;
        long for_steps;
        union {
            struct {
                long static_m;
            };
            struct {
                long ramp_m1;
                long ramp_m2;
            };
        } param;
    };
protected:
    std::vector<pattern_element> parse_pattern(const std::string &pattern);
    void show_pattern();

    std::vector<pattern_element> pattern;
    long period = 0;

    int last_p_i = -1;

    // How many steps between each time a ramp pattern reports
    // a loss change to the executor? Doesn't affect the actual parallelism at all,
    // just how often it's reported for plotting afterwards.
    const long ramp_report_interval = 128;
    long last_reported_ramp_m = -1;
public:
    PatternController(SystemExecutor &exec, std::string pattern);

    unsigned get_parallelism() override;
    void update() override;
};

}
