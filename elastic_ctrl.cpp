#include "NetworkExecutor.h"
#include <cassert>

namespace MiniDNN {

SearchController::SearchController(int num_threads, int search_phases) {
    curr_m = num_threads / 2;
    exp_phases = search_phases / 2;
    div_phases = search_phases - this->exp_phases;

    this->curr_phase = 0;
}

void SearchController::finish_phase(double loss)
{
    int next_m = -1;
    if (this->is_searching) {
        int completed_phases =
            this->curr_phase++ % (exp_phases + div_phases + 1);

        if (completed_phases < exp_phases) {
            // Exponential backoff stage
            if (this->search_prev_loss < 0 || loss < this->search_best_loss) {
                this->search_best_loss = loss;
                this->search_best_m = curr_m;

                next_m = this->curr_m + this->search_stride;

                if (this->search_stride > 0) {
                    this->search_hibound = curr_m;
                    this->hibound_loss = loss;
                } else {
                    this->search_lobound = curr_m;
                    this->lobound_loss = loss;
                }

                this->search_stride *= -1; // Flip search direction
            }
            this->search_stride *= 2;
            next_m = curr_m + this->search_stride;
        } else if (completed_phases < exp_phases + div_phases) {
            // "Divide and Conquer" stage
            if (loss < this->search_best_loss) {
                this->search_best_loss = loss;
                this->search_best_m = this->curr_m;
            }

            if (completed_phases == exp_phases) {
                // First phase of this stage
                next_m = (this->search_lobound + this->search_hibound) / 2;
            } else {
                // Already done >= 1 divide and conquer phase
                const int best_bound = lobound_loss < hibound_loss ? 
                    search_lobound : search_hibound;
                // Halfway between last probe and the best of the two bounds
                next_m = (best_bound + this->curr_m) / 2;
            }
        } else {
            this->is_searching = false;
            next_m = this->search_best_m;
        }

        this->search_prev_loss = loss;
    } else {
        // If we finished the executing phase, then begin searching again
        this->is_searching = true;
        next_m = this->search_best_m;
    }

    assert(next_m != -1);
    curr_m = next_m;
}

int SearchController::get_m()
{
    return curr_m;
}

long SearchController::target_phase_steps() {
    return this->is_searching ? 128 : 2048;
}

double SearchController::target_phase_time() {
    return -1.0;
}

/*

Searching:

 - Probe where prev. execution was

*/

}
