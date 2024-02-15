#include "NetworkExecutor.h"

void MiniDNN::NetworkExecutor::run_parallel_sync(int batch_size, int epoch, int rounds_per_epoch, int seed) {

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

    std::vector<NetworkTopology *> thread_local_networks(num_threads);
    std::vector<MultiClassEntropy *> thread_local_outputs(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        thread_local_networks[i] = new NetworkTopology(*net);
        thread_local_networks[i]->set_output(
                new MultiClassEntropy(*dynamic_cast<MultiClassEntropy *>(net->get_output())));
    }

    long step = 0;

    auto f = [&](int id) {
        long batch_index = (step * num_threads + id) % nbatch;
        thread_local_networks[id]->forward(x_batches[batch_index]);
        thread_local_networks[id]->backprop(x_batches[batch_index], y_batches[batch_index]);
        const Scalar loss = thread_local_networks[id]->get_loss();
    };

    std::vector<std::function<void(int id)>> jobs;

    for (int i = 0; i < num_threads; ++i) {
        jobs.push_back(f);
    }

    ThreadPool workers(num_threads, jobs);

    //std::cout << "wait for workers" << std::endl;
    workers.wait_for_all();
    //std::cout << "start training" << std::endl;

    // Iterations on the whole data set
    for (int k; k < epoch; k++) {

        Scalar epoch_loss = 0;

        for (int round = 0; round < rounds_per_epoch; ++round) {

            // Train on each mini-batch
            workers.start_all();
            workers.wait_for_all();

            Scalar round_loss = 0;

            for (auto sub_net: thread_local_networks) {
                round_loss += sub_net->get_loss();

                net->reset();
                net->aggregate(*sub_net);
                net->normalize_derivatives(num_threads);

                //net->update(opt); // AlignedMapVec update
                net->update_cw(opt); // component-wise update
            }

            step++;

            round_loss /= num_threads;

            std::cerr << "[Step " << step << "] Loss = " << round_loss << std::endl;

            epoch_loss += round_loss;

            std::vector<std::vector<Scalar>> agg_params = net->get_parameters();

            for (auto sub_net: thread_local_networks) {
                sub_net->set_parameters(agg_params);
            }

        }
        epoch_loss /= rounds_per_epoch;
        loss_per_epoch.push_back(epoch_loss);

        struct timeval now;
        gettimeofday(&now, NULL);
        time_per_epoch.push_back(now.tv_sec);

        //std::cerr << "[Epoch " << k << "] Loss = " << epoch_loss << std::endl;

    }

    workers.stop();
    loss = 0;
    for (int i = 0; i < num_threads; i++) {
        loss += thread_local_networks[i]->get_loss();
    }
    loss /= num_threads;
}

