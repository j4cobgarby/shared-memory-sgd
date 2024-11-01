#include "NetworkTopology.h"
#include "ParameterContainer.h"
#include "modular_components.hpp"

#include "Component/BatchController.hpp"
#include "Component/Worker.hpp"
#include "Component/Monitor.hpp"
#include "Component/Dispatcher.hpp"
#include "Component/ParaController.hpp"
#include "Component/ModelInterfaces.hpp"

#include "jsoncons/json.hpp"

#include <filesystem>

using namespace MiniDNN;
using namespace jsoncons;

int main(int argc, char *argv[]) {
    const std::string dataset_name = "CIFAR10";
    const int seed = 1337;
    const double lrate = 0.05;
    const double momentum = 0;

    NetworkTopology network(new ParameterContainer());
    SystemExecutor exec(64, 512);
    auto *batcher = new SimpleBatchController(exec, dataset_name, 32);

    if (dataset_name == "CIFAR10" || dataset_name == "CIFAR100") {
        network.add_layer(new Convolutional<ReLU>(32, 32, 3, 6, 5, 5));
        network.add_layer(new MaxPooling<ReLU>(28, 28, 6, 2, 2));

        network.add_layer(new Convolutional<ReLU>(14, 14, 6, 16, 5, 5));
        network.add_layer(new MaxPooling<ReLU>(10, 10, 16, 2, 2));

        network.add_layer(new FullyConnected<ReLU>(5 * 5 * 16, 120));
    } else if (dataset_name == "MNIST" || dataset_name == "FASHION-MNIST") {
        network.add_layer(new Convolutional<ReLU>(28, 28, 1, 6, 5, 5));
        network.add_layer(new MaxPooling<ReLU>(24, 24, 6, 2, 2));

        network.add_layer(new Convolutional<ReLU>(12, 12, 6, 16, 5, 5));
        network.add_layer(new MaxPooling<ReLU>(8, 8, 16, 2, 2));

        network.add_layer(new FullyConnected<ReLU>(4 * 4 * 16, 120));
    }

    network.add_layer(new FullyConnected<Softmax>(120, batcher->get_y_dimension()));

    network.set_output(new MultiClassEntropy());
    network.init(0, 0.01, seed);

    auto *model = new StandardModelInterface(exec, network, lrate, momentum, seed);
    auto *paracontroller = new SearchParaController(exec, 500, 3, 128, 512);
    auto *dispatcher = new AsyncDispatcher(exec);
    auto *monitor = new SlidingWindowMonitor(exec, 16);

    exec.set_model(std::shared_ptr<ModelInterface>(model));
    exec.set_batcher(std::shared_ptr<BatchController>(batcher));
    exec.set_parallelism(std::shared_ptr<ParaController>(paracontroller));
    exec.set_dispatcher(std::shared_ptr<Dispatcher>(dispatcher));
    exec.set_monitor(std::shared_ptr<Monitor>(monitor));

    /* This is created after the rest of the executor components are in place because
     * it uses the model interface when initialising the workers.
     * TODO: There must be a better way? */
    auto *workerpool = new ThreadWorkerPool<SGDWorker>(exec, 8, false);
    exec.set_workers(std::shared_ptr<WorkerPool>(workerpool));

    exec.start();

    std::cout << "[main()] Starting workers.\n";
    exec.get_workers()->start_all();

    exec.get_workers()->wait_for_all();
    std::cout << "[main()] All workers have finished.\n";

    json results;
    results["epoch_loss"] = exec.epoch_losses;
    results["para_values"] = exec.para_values;
    results["para_mstimes"] = exec.para_mstimes;
    results["epoch_mstimes"] = exec.epoch_mstimes;

    const std::filesystem::path exp_dir = "experiments";
    std::filesystem::create_directory(exp_dir);

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream fname;
    fname << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << "_results.json";
    std::filesystem::path fpath = exp_dir / fname.str();
    std::ofstream out_file(fpath);
    out_file << results;
    std::cout << "[main()] Saved results to " << fpath << std::endl;

    return 0;
}
