#include <MiniDNN.h>

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
    std::string output_folder = "./experiments";

    // Default parameters
    std::string dataset_name = "CIFAR10";
    const int seed = 1337;
    double o_lrate = 0.005;
    double o_momentum = 0;
    int o_parallelism_limit = 10;
    int o_batch_size = 16;

    // o_ variables are option parsing related.

    std::string o_para_controller = "static";
    std::string o_monitor = "ema";
    std::string o_dispatcher = "async";

    // Parameters for if we make a search controller
    // (o_*_steps also used for window controller)
    int o_search_degree = 2, o_probe_steps = 128, o_exec_steps = 512;

    // Parameters for window controller
    int o_searchwindow_size = 8;

    int o_windowsearch_m0 = -1;

    int o_semisync_period = 8000;

    SystemExecutor exec(500, 3125);

    int c;
    while ((c = getopt(argc, argv, "n:l:u:b:e:s:P:M:D:p:x:d:w:F:y:0:")) != -1) {
        switch (c) {
        case 'n':
            o_parallelism_limit = std::stoi(optarg);
            break;
        case 'l':
            o_lrate = std::stod(optarg);
            break;
        case 'u':
            o_momentum = std::stod(optarg);
            break;
        case 'b':
            o_batch_size = std::stoi(optarg);
            break;
        case 'e':
            exec._epoch_target = std::stol(optarg);
            break;
        case 's':
            exec._steps_per_epoch = std::stol(optarg);
            break;
        case 'P': // Parallelism strategy
            o_para_controller = std::string(optarg);
            break;
        case 'M':
            o_monitor = std::string(optarg);
            break;
        case 'D':
            o_dispatcher = std::string(optarg);
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
        case 'F':
            output_folder = std::string(optarg);
            break;
        case 'y':
            o_semisync_period = std::stoi(optarg);
            break;
        case '0':
            o_windowsearch_m0 = std::stoi(optarg);
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

    if (o_windowsearch_m0 <= 0) o_windowsearch_m0 = o_parallelism_limit / 2;

    NetworkTopology network(new ParameterContainer());
    auto *batcher = new SimpleBatchController(exec, dataset_name, o_batch_size);

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

    auto *model = new StandardModelInterface(exec, network, o_lrate, o_momentum, seed);

    if (o_para_controller == "ternary") {
        exec.set_parallelism(std::make_shared<SearchParaController>
                             (exec, o_parallelism_limit, o_search_degree,
                              o_probe_steps, o_exec_steps, o_searchwindow_size));
    } else if (o_para_controller == "static") {
        exec.set_parallelism(std::make_shared<StaticParaController>
                             (exec, o_parallelism_limit));
    } else if (o_para_controller == "window") {
        exec.set_parallelism(std::make_shared<WindowParaController>
                             (exec, o_parallelism_limit, o_searchwindow_size,
                              o_probe_steps, o_exec_steps, o_windowsearch_m0));
    } else if (o_para_controller == "pattern") {
        exec.set_parallelism(std::make_shared<PatternController>
                             (exec, "s 4096 200 s 4096 50 s 2048 10 s 512 200 s 512 50 s 512 200 s 512 50"));
    } else if (o_para_controller == "model") {
        exec.set_parallelism(std::make_shared<ModellingParaController>
                             (exec, o_parallelism_limit, 15, -1, 500, 8000));
    } else {
        throw std::runtime_error("Unrecognised parallelism controller name (-P)");
    }

    exec.set_model(std::shared_ptr<ModelInterface>(model));
    exec.set_batcher(std::shared_ptr<BatchController>(batcher));

    if (o_dispatcher == "async") {
        exec.set_dispatcher(std::make_shared<AsyncDispatcher>(exec));
        std::cout << "Async dispatcher made\n";
    } else if (o_dispatcher == "semisync") {
        exec.set_dispatcher(std::make_shared<SemiSyncDispatcher>(exec, o_semisync_period));
        std::cout << "Semi sync dispatcher made\n";
    } else if (o_dispatcher == "fully_sync") {
        std::cout << "For fully sync we don't use a special dispatcher. Sync worker pool is made soon\n";
        exec.set_dispatcher(std::make_shared<AsyncDispatcher>(exec));
    } else {
        throw std::runtime_error("Unrecognised dispatcher name (-D)");
    }

    if ("window" == o_monitor) {
        exec.set_monitor(std::make_shared<SlidingWindowMonitor>(exec, 2000));
    } else if ("ema" == o_monitor) {
        exec.set_monitor(std::make_shared<EMAMonitor>(exec, 0.7, false));
    } else if ("eval" == o_monitor) {
        exec.set_monitor(std::make_shared<EvalMonitor>(exec, 0.7, -1, 512, false));
    } else {
        throw std::runtime_error("Unrecognised monitor name (-M)");
    }

    /* This is created after the rest of the executor components are in place because
     * it uses the model interface when initialising the workers.
     * TODO: There must be a better way? */

    if (o_dispatcher == "fully_sync") {
        std::cout << "Creating synchronous workers.\n";
        auto *workerpool = new ThreadWorkerPoolAsync<SGDWorkerSynchronous>(exec, o_parallelism_limit, false);
        exec.set_workers(std::shared_ptr<WorkerPool>(workerpool));
    } else {
        std::cout << "Creating asynchronous workers.\n";
        auto *workerpool = new ThreadWorkerPoolAsync<SGDWorkerAsync>(exec, o_parallelism_limit, false);
        exec.set_workers(std::shared_ptr<WorkerPool>(workerpool));
    }

    exec.start();

    json results;
    results["epoch_loss"] = exec._epoch_losses;
    results["para_values"] = exec._para_values;
    results["para_mstimes"] = exec._para_mstimes;
    results["epoch_mstimes"] = exec._epoch_mstimes;
    results["steptimes"] = exec._steptime_samples;
    results["tau_dist"] = exec._tau_dist;
    results["step_acceptance_rate"] = (double)exec._accepted_steps / (double)(exec._accepted_steps + exec._rejected_steps);

    json meta;
    meta["learning_rate"] = o_lrate;
    meta["momentum"] = o_momentum;
    meta["num_threads"] = o_parallelism_limit;
    meta["batch_size"] = o_batch_size;
    meta["num_epochs"] = exec._epoch_target;
    meta["epoch_steps"] = exec._steps_per_epoch;
    meta["probe_steps"] = o_probe_steps;
    meta["exec_steps"] = o_exec_steps;
    meta["search_degree"] = o_search_degree;
    meta["para_ctrl"] = o_para_controller;
    meta["monitor"] = o_monitor;
    meta["dispatcher"] = o_dispatcher;
    meta["window_size"] = o_searchwindow_size;
    meta["semisync_period"] = o_semisync_period;

    results["meta"] = meta;

    const std::filesystem::path exp_dir = output_folder;
    create_directory(exp_dir);

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
