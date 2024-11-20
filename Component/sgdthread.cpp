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
#if MEASURE_STEP_TIME
            auto t1 = HRClock::now();
#endif
            // Get batch from batch controller
            int batch_id = exec.get_batcher()->get_batch_ind(this->id);
            const Matrix &b_x = exec.get_batcher()->get_batch_data(batch_id);
            const Matrix &b_y = exec.get_batcher()->get_batch_labels(batch_id);

            // Calculate a gradient based on this batch (getting loss)
            auto *local_param = new ParameterContainer(*global_param_ptr);

            this->network->set_pointer(local_param);
            this->network->forward(b_x);
            this->network->backprop(b_x, b_y);

            exec.get_dispatcher()->finish_step(this->id);

            // Give loss to monitor
            exec.get_monitor()->update(this->network->get_loss());

            // Apply gradient to model interface 
            // TODO: This section should really be delegated to the ModelInterface
            this->network->set_pointer(global_param_ptr);
            delete local_param;

            this->network->update_cw(this->optim.get());

#if MEASURE_STEP_TIME
            auto t2 = HRClock::now();
            long x = (t2 - t1).count();

            if (steptime_samples.size() < N_STEP_TIME_SAMPLES)
                steptime_samples.push_back(x);

            steptime_n++;

            steptime_min = std::min(steptime_min, x);
            steptime_max = std::max(steptime_max, x);

            auto new_running_avg = steptime_running_avg + (x - steptime_running_avg) / steptime_n;
            steptime_sum_of_squares += (x - steptime_running_avg) * (x - new_running_avg);

            steptime_running_avg = new_running_avg;
#endif
        }
    }

#if MEASURE_STEP_TIME
    std::this_thread::sleep_for(std::chrono::milliseconds(this->id * 100)); // Don't all print at once
    // jsoncons::json st_json;
    // st_json["avg"] = steptime_running_avg;
    // st_json["variance"] = steptime_sum_of_squares / steptime_n;
    // st_json["N"] = steptime_n;
    // st_json["max"] = steptime_max;
    // st_json["min"] = steptime_min;
    //
    // std::cout << st_json << "," << std::endl;
    for (long x : steptime_samples) {
        std::cout << "," << x;
    }
#endif
}

}
