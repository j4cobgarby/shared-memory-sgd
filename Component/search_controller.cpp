#include "modular_components.hpp"
#include "Component/ParaController.hpp"

namespace MiniDNN {

SearchParaController::SearchParaController(SystemExecutor &exec, const int num_threads,
                                           const int search_degree,
                                           long probe_steps, long exec_steps,
                                           const int window_size) :
    ParaController(exec),
    is_searching(true),
    search_degree(search_degree),
    low_bound(num_threads/2 - window_size/2),
    high_bound(low_bound + window_size - 1),
    probe_steps(probe_steps),
    exec_steps(exec_steps),
    total_workers(num_threads),
    window_size(window_size) {

    switch_to_para(low_bound + (unsigned)(0.25 * (high_bound - low_bound)));
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

    clip_window();
}

void SearchParaController::switch_to_para(const unsigned m) {
    this->curr_parallelism = m;

    exec.mtx_para_vec.lock();
    exec.para_mstimes.push_back(exec.elapsed_time());
    exec.para_values.push_back(m);
    exec.mtx_para_vec.unlock();

    this->t_stage_start = HRClock::now();
}

void SearchParaController::clip_window() {
    /* Clip window to be within bounds */
    int sz = 1 + this->high_bound - this->low_bound;
    if (this->low_bound < 1) {
        this->low_bound = 1;
        this->high_bound = sz;
    }

    if (this->high_bound > this->total_workers) {
        this->high_bound = this->total_workers;
        this->low_bound = 1 + this->total_workers - sz;
    }
}

void SearchParaController::update(const long step) {
    // Should we be switching to searching now?
    if (!is_searching && step - phase_start_step >= exec_steps) {
        is_searching = true;
        phase_start_step = step;
        probe_counter = 0;

        /* Bound the search region to exec parallelism +/- W/2 */
        low_bound = curr_parallelism - (window_size/2);
        high_bound = low_bound + window_size - 1;
        clip_window();

        switch_to_para(low_bound + (unsigned)(0.25 * (high_bound - low_bound)));

        std::cout << "[search] Switched to searching from execution." << std::endl;
        std::cout << "[search] Whole window is " << low_bound << ".." << high_bound << std::endl;
        return;
    }


    if (is_searching) {
        // We need only one thread at a time to check if the stage should progress, otherwise
        // consecutive steps (all > stage duration) can trigger individual progressions.
        this->mtx.lock();
        const bool should_progress = step - this->phase_start_step >= this->probe_steps;
        if (should_progress) this->phase_start_step = step;
        this->mtx.unlock();

        if (should_progress) {
            std::cout << "[search] At step " << step << " we are finshing a probe.\n";
            // We've got to the end of one of the probes
            const double loss_compd = exec.get_monitor()->get_loss_accur();
            const double stage_dur_s = (double)(HRClock::now() - this->t_stage_start)
                .count()
                * 1e-9;
            const double stage_convrate = (loss_compd - this->loss_start_of_stage)
                / stage_dur_s;
            this->loss_start_of_stage = loss_compd;

            if (stage_convrate < best_convrate) {
                best_probe = probe_counter % 3;
                best_convrate = stage_convrate;
                best_probe_m = curr_parallelism;
                std::cout << "New best probe in stage: m=" << best_probe_m << ", ind=" << best_probe
                    << ", rate=" << best_convrate << std::endl;
            }

            probe_counter++;

            if (probe_counter % 3 == 0) {
                // Here we're beginning a new search level, so make sure to shrink the bounds based
                // on the previous level (as long as this isn't the first one).
                if (probe_counter > 0) shrink_bounds();

                // Reset the best found loss, since we want to find the best probing loss within one
                // search level.
                best_convrate = std::numeric_limits<double>::infinity();

                // If we would start a new search level, but we've done enough now, then we stop
                // searching and hence begin an execution phase.
                if (probe_counter == 3 * search_degree) {
                    is_searching = false;
                    switch_to_para(best_probe_m);

                    std::cout << "[search] Switched to execution with m=" << curr_parallelism << std::endl;
                    return;
                }

                switch_to_para(low_bound + (unsigned)(0.25 * (high_bound - low_bound)));
                std::cout << "[search] Switched to low probe with m=" << curr_parallelism << std::endl;
            } else if (probe_counter % 3 == 1) {
                // Second (1/2) probe in the same search level
                switch_to_para(low_bound + (unsigned)(0.5 * (high_bound - low_bound)));

                std::cout << "[search] Switched to mid probe with m=" << curr_parallelism << std::endl;
            } else {
                // Third (3/4) probe
                switch_to_para(low_bound + (unsigned)(0.75 * (high_bound - low_bound)));
                std::cout << "[search] Switched to high probe with m=" << curr_parallelism << std::endl;
            }
        }
    }
}

}
