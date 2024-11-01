#include "modular_components.hpp"
#include "Component/ParaController.hpp"

namespace MiniDNN {

SearchParaController::SearchParaController(SystemExecutor &exec, const int num_threads,
                                           const int search_degree,
                                           long probe_steps, long exec_steps) :
    ParaController(exec),
    is_searching(true),
    search_degree(search_degree),
    high_bound(num_threads),
    probe_steps(probe_steps),
    exec_steps(exec_steps),
    total_threads(num_threads) {

    curr_parallelism = (unsigned)(0.33 * num_threads);
    phase_start_step = 0;
}

unsigned SearchParaController::get_parallelism() {
    return this->curr_parallelism;
}

void SearchParaController::shrink_bounds() {
    const int new_width = (high_bound - low_bound) / 2;
    switch (best_probe) {
    case 0:
        high_bound = low_bound + new_width;
        break;
    case 1:
        low_bound += new_width / 2;
        high_bound = low_bound + new_width;
        break;
    case 2:
        low_bound = high_bound - new_width - 1;
        break;
    default:
        std::cerr << "bug: best_probe not in {0,1,2}\n";
        exit(-1);
    }
}

void SearchParaController::update() {
    const long steps_done = exec.get_dispatcher()->get_steps_done();

    // Should we be switching to searching now?
    if (!is_searching && steps_done - phase_start_step >= exec_steps) {
        is_searching = true;
        phase_start_step = steps_done;
        high_bound = total_threads;
        probe_counter = 0;
        curr_parallelism = low_bound + (unsigned)(0.25 * (high_bound - low_bound));

        std::cout << "[search] Switched to searching from execution." << std::endl;
        return;
    }

    if (is_searching) {
        if (steps_done - phase_start_step >= probe_steps) {
            std::cout << "[search] At step " << steps_done << " we are finshing a probe.\n";
            // We've got to the end of one of the probes
            if (const double loss = exec.get_monitor()->get_loss(); loss < best_probe_loss) {
                best_probe = probe_counter % 3;
                best_probe_loss = loss;
                best_probe_m = curr_parallelism;
                std::cout << "New best probe in stage: m=" << best_probe_m << ", ind=" << best_probe << ", loss=" << best_probe_loss << std::endl;
            }

            probe_counter++;

            if (probe_counter % 3 == 0) {
                // Here we're beginning a new search level, so make sure to shrink the bounds based
                // on the previous level (as long as this isn't the first one).
                if (probe_counter > 0) shrink_bounds();

                // Reset the best found loss, since we want to find the best probing loss within one
                // search level.
                best_probe_loss = std::numeric_limits<double>::infinity();

                // If we would start a new search level, but we've done enough now, then we stop
                // searching and hence begin an execution phase.
                if (probe_counter == 3 * search_degree) {
                    is_searching = false;
                    phase_start_step = steps_done;
                    curr_parallelism = best_probe_m;
                    std::cout << "[search] Switched to execution with m=" << curr_parallelism << std::endl;
                    return;
                }

                curr_parallelism = low_bound + (unsigned)(0.25 * (high_bound - low_bound));
                std::cout << "[search] Switched to low probe with m=" << curr_parallelism << std::endl;
            } else if (probe_counter % 3 == 1) {
                // Second (1/2) probe in the same search level
                curr_parallelism = low_bound + (unsigned)(0.5 * (high_bound - low_bound));

                std::cout << "[search] Switched to mid probe with m=" << curr_parallelism << std::endl;
            } else {
                // Third (3/4) probe
                curr_parallelism = low_bound + (unsigned)(0.75 * (high_bound - low_bound));
                std::cout << "[search] Switched to high probe with m=" << curr_parallelism << std::endl;
            }

            phase_start_step = steps_done;
        }
    }
}

}