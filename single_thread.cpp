#include "NetworkExecutor.h"


/// \param epoch      Number of epochs of training.
/// \param seed       Set the random seed of the %RNG if `seed > 0`, otherwise
///                   use the current random state.
//
void MiniDNN::NetworkExecutor::run_training(int batch_size, int epoch, int rounds_per_epoch, int seed) {

    opt->reset();

    // Create shuffled mini-batches
    if (seed > 0) {
        m_rng.seed(seed);
    }

    std::vector<XType> x_batches;
    std::vector<YType> y_batches;
    //std::vector<MultiClassEntropy*> output_layers;
    const int nbatch = internal::create_shuffled_batches(x, y, batch_size, m_rng,
                                                            x_batches, y_batches);

    if (rounds_per_epoch < 0) {
        rounds_per_epoch = nbatch;
    }

    long step = 0;

    for (int k = 0; k < epoch; k++) {

        Scalar epoch_loss = 0;

        for (int round = 0; round < rounds_per_epoch; ++round) {
            long batch_index = step % nbatch;
            net->forward(x_batches[batch_index]);
            net->backprop(x_batches[batch_index], y_batches[batch_index]);
            const Scalar loss = net->get_loss();
            //std::cerr << "[Step " << step << "] Loss = " << loss << std::endl;

            epoch_loss += loss;

            //net->update(opt); // AlignedMapVec update
            net->update_cw(opt); // component-wise update

            step++;
        }

        epoch_loss /= rounds_per_epoch;
        loss_per_epoch.push_back(epoch_loss);

        struct timeval now;
        gettimeofday(&now, NULL);
        time_per_epoch.push_back(now.tv_sec);

        //std::cerr << "[Epoch " << k << "] Loss = " << epoch_loss << std::endl;

    }
}
