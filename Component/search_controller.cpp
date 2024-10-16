#include "modular_components.hpp"
#include "Component/ParaController.hpp"

namespace MiniDNN {

SearchController::SearchController(SystemExecutor &exec, int num_threads, int search_phases) : ParaController(exec) {
    curr_m = num_threads / 2;
    exp_phases = search_phases / 2;
    div_phases = search_phases - this->exp_phases;

    this->curr_phase = 0;
}

unsigned SearchController::get_parallelism() {
    return curr_m;
}


}
