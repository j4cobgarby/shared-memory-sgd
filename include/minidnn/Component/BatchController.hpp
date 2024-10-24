#pragma once

#include "minidnn/modular_components.hpp"

namespace MiniDNN {

class SimpleBatchController : public BatchController {
protected:
    int next_batch = 0;

    RNG rng;
public:
    SimpleBatchController(SystemExecutor &exec, std::string dataset, int batch_size);

    int get_batch_ind(long worker_id) {
        int ret = next_batch++;
        next_batch %= this->x_batches.size();
        return ret;
    }
};

}

