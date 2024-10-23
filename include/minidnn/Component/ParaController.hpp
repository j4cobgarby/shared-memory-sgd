#pragma once

#include "modular_components.hpp"

namespace MiniDNN {

/* A simple parallelism controller which maintains a static parallelism at all times */
class StaticParaController : public ParaController {
private:
    unsigned m;
public:
    StaticParaController(SystemExecutor &exec, unsigned m) : ParaController(exec), m(m) {}
    unsigned get_parallelism() { return m; }
};

class SearchParaController : public ParaController {
private:
    bool is_searching;

    //// Searching Variables
    // Phase within current search, set to 0 when searching starts
    int curr_phase;
    int search_lobound;
    int search_hibound;

    int search_stride = 5;
    double search_best_loss = -1;
    double search_prev_loss = -1;
    int search_best_m; 

    int curr_m = -1;
public:
    SearchParaController(SystemExecutor &exec, int num_threads, int search_phases);

    unsigned get_parallelism() override;
};

}
