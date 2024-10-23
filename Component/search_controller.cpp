#include "modular_components.hpp"
#include "Component/ParaController.hpp"

namespace MiniDNN {

SearchParaController::SearchParaController(SystemExecutor &exec, int num_threads, int search_phases) : ParaController(exec) {
    curr_m = num_threads / 2;

    this->curr_phase = 0;
}

unsigned SearchParaController::get_parallelism() {
    return curr_m;
}

}
