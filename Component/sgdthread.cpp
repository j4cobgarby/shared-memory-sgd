#include "ParameterContainer.h"
#include "minidnn/Component/Worker.hpp"

namespace MiniDNN {

void SGDThread::run() {
    while (!exec.get_dispatcher()->is_finished()) {
        if (!exec.get_dispatcher()->try_start_step(this->id)) {
            // Get batch from batch controller
            int batch_id = exec.get_batcher()->get_batch_ind(this->id);
            const Matrix &b_x = exec.get_batcher()->get_batch_data(this->id);
            const Matrix &b_y = exec.get_batcher()->get_batch_labels(this->id);

            // Calculate a gradient based on this batch (getting loss)
            auto *local_param = new ParameterContainer(*exec.get_model()->get_param());

            this->network.set_pointer(local_param);
            this->network.forward(b_x);
            this->network.backprop(b_x, b_y);

            // Give loss to monitor
            exec.get_monitor()->update(this->network.get_loss());

            // Apply gradient to model interface 
            this->network.set_pointer(exec.get_model()->get_param().get());
            delete local_param;

            exec.get_dispatcher()->finish_step(this->id);
        }
    }
}

}
