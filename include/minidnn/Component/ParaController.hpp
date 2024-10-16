#pragma once

#include "modular_components.hpp"

namespace MiniDNN {

/* This file defines all the types of ParaController that we implement. */

class SearchController : public ParaController {
private:
    bool is_searching;

    //// Searching Variables
    // Phase within current search, set to 0 when searching starts
    int curr_phase;
    int search_lobound;
    int search_hibound;
    double lobound_loss;
    double hibound_loss;
    int search_stride = 5;
    double search_best_loss = -1;
    double search_prev_loss = -1;
    int search_best_m; 

    int exp_phases;
    int div_phases;

    int curr_m = -1;
public:
    SearchController(SystemExecutor &exec, int num_threads, int search_phases);

    unsigned get_parallelism() override;
};

}
