#include "modular_components.hpp"
#include "Component/ParaController.hpp"
#include "utils.h"
#include <limits>

namespace MiniDNN {

WindowParaController::WindowParaController(SystemExecutor &exec, const int num_threads,
                                           const int window_size,
                                           long probe_steps, long exec_steps) :
    ParaController(exec),
    window_size(window_size),
    probe_steps(probe_steps),
    exec_steps(exec_steps),
    total_workers(num_threads),
    phase_start_step(0),
    best_convrate(std::numeric_limits<double>::infinity()),
    window_btm(num_threads/2 - window_size/2) {
    clip_window();
    switch_to_para(this->window_btm);
}

unsigned WindowParaController::get_parallelism() {
    return this->curr_parallelism;
}

void WindowParaController::switch_to_para(const unsigned m) {
    // Report the new m value to the executor, for recording data
    this->curr_parallelism = this->exec.submit_para_change(m);
}

void WindowParaController::clip_window() {
        /* Clip window to be within bounds */
        if (this->window_btm < 1)
            this->window_btm = 1;

        if (this->window_btm + this->window_size >= this->total_workers)
            this->window_btm = 1 + this->total_workers - this->window_size;
}

/* This gets called when the monitor receives an update */
void WindowParaController::update() {
    const long steps_done = exec.get_dispatcher()->get_steps_done();

    if (!this->is_probing && steps_done - this->phase_start_step >= this->exec_steps) {
        this->is_probing = true;
        this->phase_start_step = steps_done;
        this->window_btm = this->curr_parallelism - (this->window_size / 2);

        /* Compute the loss at the start of the first probing stage of this phase */
        this->loss_start_of_stage = exec.get_monitor()->get_loss_accur();

        clip_window();

        switch_to_para(this->window_btm);

        std::cout << "[window_probe] START_PROBING window_btm="
            << this->window_btm << std::endl;
    }

    if (this->is_probing) {
        /*  Have we just finished a probing stage? */
        if (steps_done - this->phase_start_step >= this->probe_steps) {
            const double loss_compd = exec.get_monitor()->get_loss_accur();
            /* Now we have an accurate idea of the current loss, but we want to know how this
             * compares to the loss at the start of the probing stage, AND then divide that
             * by how long the stage took! */

            const double stage_dur_ns = (double)(HRClock::now() - this->t_stage_start)
                .count() // To ns (long)
                * 1e-9;  // To seconds (double)
            const double stage_convrate = (loss_compd - this->loss_start_of_stage)
                / stage_dur_ns;

            /* The next stage uses this stage's end loss as its starting loss */
            this->loss_start_of_stage = loss_compd;

            std::cout << "[window_probe] PROBE_DONE @step " << steps_done
                << " LOSS=" << loss_compd << ", Rate=" << stage_convrate << "\n";

            if (stage_convrate < this->best_convrate) {
                best_convrate = loss_compd;
                best_probe_m = curr_parallelism;
                std::cout << "[window_probe] BEST_PROBE m=" << best_probe_m
                    << " Rate=" << best_convrate << std::endl;
            }

            if (this->curr_parallelism >= this->window_btm + this->window_size) {
                /* We've just finished the window, so switch to execution */
                this->is_probing = false;
                this->phase_start_step = steps_done;
                this->best_convrate = std::numeric_limits<double>::infinity();
                switch_to_para(best_probe_m);

                std::cout << "[window_probe] START_EXEC m=" <<
                    this->curr_parallelism << std::endl;

                return;
            }

            switch_to_para(this->curr_parallelism + 1);
            this->phase_start_step = steps_done;
            std::cout << "[window_probe] PROBE_START m=" <<
                this->curr_parallelism << std::endl;
        }
    }
}

}
