#include "modular_components.hpp"
#include "Component/ParaController.hpp"
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
    best_probe_loss(std::numeric_limits<double>::infinity()),
    window_btm(num_threads/2 - window_size/2) {
    clip_window();
    switch_to_para(this->window_btm);
}

unsigned WindowParaController::get_parallelism() {
    return this->curr_parallelism;
}

void WindowParaController::switch_to_para(const unsigned m) {
    this->curr_parallelism = m;

    exec.mtx_para_vec.lock();
    exec.para_mstimes.push_back(exec.elapsed_time());
    exec.para_values.push_back(m);
    exec.mtx_para_vec.unlock();
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

        clip_window();

        switch_to_para(this->window_btm);

        std::cout << "[window_probe] START_PROBING window_btm="
            << this->window_btm << std::endl;
    }

    if (this->is_probing) {
        if (steps_done - this->phase_start_step >= this->probe_steps) {
            const double loss = exec.get_monitor()->get_loss();
            std::cout << "[window_probe] PROBE_DONE @step " << steps_done 
                << " LOSS=" << loss << std::endl;

            if (loss < this->best_probe_loss) {
                best_probe_loss = loss;
                best_probe_m = curr_parallelism;
                std::cout << "[window_probe] BEST_PROBE m=" << best_probe_m
                    << " LOSS=" << best_probe_loss << std::endl;
            }

            if (this->curr_parallelism >= this->window_btm + this->window_size) {
                /* We've just finished the window, so switch to execution */
                this->is_probing = false;
                this->phase_start_step = steps_done;
                this->best_probe_loss = std::numeric_limits<double>::infinity();
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
