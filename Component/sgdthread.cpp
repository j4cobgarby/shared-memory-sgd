#include "ParameterContainer.h"
#include "jsoncons/basic_json.hpp"
#include "minidnn/Component/Worker.hpp"
#include <chrono>
#include <thread>

namespace MiniDNN {

void SGDWorker::run() {
    auto global_param_ptr = exec.get_model()->get_network()->current_param_container_ptr;

    /* Delay thread execution until flag is tripped */
    this->flag->wait(true);

    // If this starts running before the Dispatcher is ready to actually start
    // (which is likely since threads are created at initialisation), that's okay
    // as long as Dispatcher.is_finished() is not true.
    // Since try_start_step could initially block until the Dispatcher is ready,
    // this therefore doesn't have to involve busy waiting.
    while (!exec.get_dispatcher()->is_finished()) {
        if (exec.get_dispatcher()->try_start_step(this->id)) {
            const auto t1 = HRClock::now();

            // Get batch from batch controller
            const int batch_id = exec.get_batcher()->get_batch_ind(this->id);
            const Matrix &b_x = exec.get_batcher()->get_batch_data(batch_id);
            const Matrix &b_y = exec.get_batcher()->get_batch_labels(batch_id);

            // Calculate a gradient based on this batch (getting loss)
            auto *local_param = new ParameterContainer(*global_param_ptr);

            this->network->set_pointer(local_param);
            this->network->forward(b_x);
            this->network->backprop(b_x, b_y);

            const long finished_step = exec.get_dispatcher()->finish_step(this->id);

            // Apply gradient to model interface 
            // TODO: This section should really be delegated to the ModelInterface
            this->network->set_pointer(global_param_ptr);
            delete local_param;

            this->network->update_cw(this->optim.get());

            const auto t2 = HRClock::now();
            const long x = (t2 - t1).count();

            // Give loss to monitor
            exec.get_monitor()->update(this->network->get_loss(), x, finished_step);

#if MEASURE_STEP_TIME
            /* If we want to print all the measured time samples afterwards, we have to store them. */

            // Append new samples, up to vector's reserved size
            if (steptime_samples.size() < N_STEP_TIME_SAMPLES) {
                const auto t_start = exec.start_time_hr;
                steptime_samples.emplace_back((t1-t_start).count(), (t2-t_start).count());
            }
#endif
        }
    }

#if MEASURE_STEP_TIME
    exec.submit_steptimes(steptime_samples);
    // std::this_thread::sleep_for(std::chrono::milliseconds(this->id * 100)); // Don't all print at once
    // for (const auto [t1, t2] : steptime_samples) {
    //     std::cout << "[" << t1 << "," << t2 << "],";
    // }
#endif
}

}
