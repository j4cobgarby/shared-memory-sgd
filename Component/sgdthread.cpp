#include "ParameterContainer.h"
#include "minidnn/Component/Worker.hpp"

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
        }
    }
}

}
