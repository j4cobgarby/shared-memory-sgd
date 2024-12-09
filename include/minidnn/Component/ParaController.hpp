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
    unsigned get_latest_exec_parallelism() override { return m; }
    void update(long step) override {}
};

class WindowParaController : public ParaController {
    /* Probing logic */
    bool is_probing = true;
    unsigned curr_parallelism;
    unsigned latest_exec_parallelism;
    long phase_start_step = -1;
    int window_btm;
    unsigned best_probe_m;
    double best_convrate;

    /* Used for calculating delta loss/T */
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
    unsigned get_latest_exec_parallelism() override { return latest_exec_parallelism; }
    void update(long step) override;
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
    double best_convrate = std::numeric_limits<double>::infinity();

    /* Used for calculating delta loss/T */
    std::chrono::time_point<HRClock> t_stage_start;
    double loss_start_of_stage;

    unsigned curr_parallelism = 0;
    unsigned latest_exec_parallelism;

    void shrink_bounds();
    void switch_to_para(unsigned m);
    void clip_window();
public:
    SearchParaController(SystemExecutor &exec, int num_threads, int search_degree,
        long probe_steps, long exec_steps, int window_size);

    unsigned get_parallelism() override;
    unsigned get_latest_exec_parallelism() override { return latest_exec_parallelism; }

    void update(long step) override;
};

class ModellingParaController : public ParaController {
    bool is_probing = true;
    long phase_start_step = 0;
    int current_probe_number = 1;

    unsigned curr_parallelism = -1;
    unsigned latest_exec_parallelism = -1;

    std::chrono::time_point<HRClock> t_stage_start;
    double loss_start_of_stage = 0;

    const long probe_steps, exec_steps;
    const int num_probes;
    const int total_workers;
    const int window_size;

    std::vector<double> probed_rates;
    std::vector<double> probed_m_values;

    void clip_window();
    void switch_to_para(unsigned m);
    double estimate_min(std::vector<double> &xs, std::vector<double> &ys, int degree, double min_x, double max_x);
    std::pair<double, double> find_polynomial_min(const std::vector<double>& coeffs, double min_x, double max_x);
    double eval_polynomial(const std::vector<double>& coeffs, double at_x);
public:
    ModellingParaController(SystemExecutor &exec, int num_threads, int num_probes, int window_size,
        long probe_steps, long exec_steps);

    unsigned get_parallelism() override { return curr_parallelism; }
    unsigned get_latest_exec_parallelism() override { return latest_exec_parallelism; }
    void update(long step) override;
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
    PatternController(SystemExecutor &exec, const std::string& pattern);

    unsigned get_parallelism() override;
    unsigned get_latest_exec_parallelism() override { return 1; }
    void update(long step) override;
};

}
