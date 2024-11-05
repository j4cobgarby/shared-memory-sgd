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
#include <memory>
#include <string>

using namespace MiniDNN;
using namespace jsoncons;

int main(int argc, char *argv[]) {
    const std::string dataset_name = "CIFAR10";
    const int seed = 1337;
    double lrate = 0.005;
    double momentum = 0;
    int parallelism_limit = -1;

    // o_ variables are option parsing related.

    std::string o_para_controller = "static";

    // Parameters for if we make a search controller
    // (o_*_steps also used for window controller)
    int o_search_degree = 2, o_probe_steps = 128, o_exec_steps = 512;

    // Parameters for window controller
    int o_searchwindow_size = 8;

    SystemExecutor exec(500, 1024);

    int c;
    while ((c = getopt(argc, argv, "n:l:u:e:s:P:p:x:d:w:")) != -1) {
        switch (c) {
        case 'n':
            parallelism_limit = std::stoi(optarg);
            break;
        case 'l':
            lrate = std::stod(optarg);
            break;
        case 'u':
            momentum = std::stod(optarg);
            break;
        case 'e':
            exec.epoch_target = std::stol(optarg);
            break;
        case 's':
            exec.steps_per_epoch = std::stol(optarg);
            break;
        case 'P': // Parallelism strategy
            o_para_controller = std::string(optarg);
            break;
        case 'p': // probing steps
            o_probe_steps = std::stoi(optarg);
            break;
        case 'x': // execution steps
            o_exec_steps = std::stoi(optarg);
            break;
        case 'd':
            o_search_degree = std::stoi(optarg);
            break;
        case 'w':
            o_searchwindow_size = std::stoi(optarg);
            break;
        case '?':
            std::cout << "Unknown option: " << optopt << std::endl;
            std::exit(-1);
        default:
            std::cerr << "default case in getopt switch.\n";
            std::cout << "Option: " << optopt << std::endl;
            std::exit(-1);
        }
    }

    NetworkTopology network(new ParameterContainer());
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
    // auto *paracontroller = new SearchParaController(exec, 48, 2, 128, 512);
    auto *dispatcher = new AsyncDispatcher(exec);
    auto *monitor = new SlidingWindowMonitor(exec, 16);

    if (o_para_controller == "ternary") {
        exec.set_parallelism(std::make_shared<SearchParaController>
                             (exec, parallelism_limit, o_search_degree,
                              o_probe_steps, o_exec_steps));
    } else if (o_para_controller == "static") {
        exec.set_parallelism(std::make_shared<StaticParaController>
                             (exec, parallelism_limit));
    } else if (o_para_controller == "window") {
        exec.set_parallelism(std::make_shared<WindowParaController>
                             (exec, parallelism_limit, o_searchwindow_size,
                              o_probe_steps, o_exec_steps));
    } else {
        std::cerr << "parallelism controller name unrecognised.\n";
    }

    exec.set_model(std::shared_ptr<ModelInterface>(model));
    exec.set_batcher(std::shared_ptr<BatchController>(batcher));
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

    json meta;
    meta["learning_rate"] = lrate;
    meta["momentum"] = momentum;
    meta["num_threads"] = parallelism_limit;
    meta["batch_size"] = 32;
    meta["num_epochs"] = exec.epoch_target;
    meta["epoch_steps"] = exec.steps_per_epoch;
    meta["probe_steps"] = o_probe_steps;
    meta["exec_steps"] = o_exec_steps;
    meta["search_degree"] = o_search_degree;
    meta["para_ctrl"] = o_para_controller;

    results["meta"] = meta;

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
