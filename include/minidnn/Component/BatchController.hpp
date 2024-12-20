#pragma once

#include "minidnn/modular_components.hpp"

namespace MiniDNN {

class SimpleBatchController : public BatchController {
protected:
    std::atomic<unsigned long> next_batch = 0;
    const int bs;

    RNG rng;
public:
    SimpleBatchController(SystemExecutor &exec, std::string dataset, int batch_size);

    unsigned long get_batch_ind(long worker_id, std::unique_ptr<int> batch_size_out) override
    {
        const unsigned long ret = next_batch.fetch_add(1);
        *batch_size_out = bs;
        return ret % this->_x_batches.size();
    }
};

class ElasticBatchController : public BatchController {
protected:
    /* This batch controller produces batches of varying sizes by gluing together a number of
     * smaller batches. This makes the implementation way simpler. Here, we set the minimum
     * possible batch size. Setting this way smaller than necessary (e.g. 1) does of course give
     * more options for effective batch sizes, but at some extra overhead. */
    const int min_bs = 8;
    int current_bs; // Should always be a multiple of min_bs
    RNG rng;
    std::atomic<unsigned long> next_batch_start = 0;

    Matrix merge_batches(const std::vector<Matrix>& vec, const unsigned long first, const int count) const
    {
        Matrix ret(vec.at(0).rows(), vec.at(0).cols() * count);
        for (int off = 0; off < count; off++) {
            const unsigned long b_ind = (first + off) % vec.size();
            // Copy matrix at vec[b_ind] to ret, starting at ret's (b_ind * min_bs)th column
            ret.block(0, (long)b_ind * min_bs, vec.at(b_ind).rows(), min_bs) = vec.at(b_ind);
        }
        return ret;
    }
public:
    ElasticBatchController(SystemExecutor &exec, std::string dataset, int initial_batch_size);

    unsigned long get_batch_ind(long worker_id, std::unique_ptr<int> batch_size_out) override {
        const int bs = current_bs;
        const int num_units = bs / min_bs;

        const unsigned long ret = next_batch_start.fetch_add(num_units);
        *batch_size_out = bs;
        return ret % this->_x_batches.size();
    }

    Matrix get_batch_data(const unsigned long id, const int batch_size) override {
        if (batch_size % min_bs != 0) {
            throw std::runtime_error("Requested batch of size not divisible by unit batch size.");
        }
        return std::move(merge_batches(_x_batches, id, batch_size / min_bs));
    }

    Matrix get_batch_labels(const unsigned long id, const int batch_size) override {
        if (batch_size % min_bs != 0) {
            throw std::runtime_error("Requested batch of size not divisible by unit batch size.");
        }
        return std::move(merge_batches(_y_batches, id, batch_size / min_bs));
    }
};

}
