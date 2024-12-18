#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <sys/time.h>
#include <type_traits>
#include <fstream>

#include <MiniDNN.h>
#include <mnist.h>

#include <ParameterContainer.h>

#include <NetworkExecutor.h>

#include "Executors.hpp"
#include "cifar10_reader.hpp"
#include "cifar100_reader.hpp"

#include <jsoncons/json.hpp>

using namespace MiniDNN;

int rand_seed = 1337;
double learning_rate;
Scalar momentum_mu;
int num_threads, batch_size, num_epochs, run_n;
int num_hidden_layers = 1;
int num_hidden_units = 128;
int rounds_per_epoch = -1;
int cas_backoff = 200;
bool check_concurrent_updates = 0;
int probing_duration = -1;
int probing_interval = -1;
int probing_window = 1;
int initial_parallelism = -1;
double heuristic_gradient = 0;
double scalar_loss_grad = 0,
       scalar_loss_jitter = 0,
       scalar_m_trend = 0;

enum class ALGORITHM {
    ASYNC, HOG, LSH, SEQ, SYNC, ELASYNC, SEMISYNC, HEURISTIC,
};
std::vector<std::string> AlgoTypes = {
        "ASYNC",
        "HOG",
        "LSH",
        "SEQ",
        "SYNC",
        "ELASYNC",
        "SEMISYNC",
        "HEURISTIC",
};
ALGORITHM run_algo = ALGORITHM::SEQ;

enum class ARCHITECTURE {
    MLP, CNN, LENET
};
std::vector<std::string> ArchTypes = {
        "MLP",
        "CNN",
        "LENET"
};
ARCHITECTURE use_arch = ARCHITECTURE::MLP;

std::string tauadaptstrat = "NONE";

std::string use_dataset = "MNIST";
std::string algo_name;
std::string arch_name;
std::string experiment_name;

template<typename T>
std::ostream &operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type &stream, const T &e) {
    return stream << static_cast<typename std::underlying_type<T>::type>(e);
}

int main(int argc, char *argv[]) {
    struct option long_options[] = {
            // These options don't set a flag
            {"help",                no_argument,       nullptr, 'h'},
            {"algo",                required_argument, nullptr, 'a'},
            {"arch",                required_argument, nullptr, 'A'},
            {"epochs",              required_argument, nullptr, 'e'},
            {"num-threads",         required_argument, nullptr, 'n'},
            {"print-vals",          required_argument, nullptr, 'v'},
            
            {nullptr, 0,                                  nullptr, 0}
    };

    if (argc == 1) {
        printf("Use -h or --help for help\n");
        exit(0);
    }
    int i, c;

    unsigned long arch_id;

    while (1) {
        i = 0;
        c = getopt_long(argc, argv,
                        "a:b:e:n:r:l:m:B:R:C:A:N:L:U:t:D:w:i:d:s:G:J:M:", long_options, &i);

        if (c == -1) {
            //printf("Use -h or --help for help\n");
            //exit(0);
            break;
        }

        if (c == 0 && long_options[i].flag == 0)
            c = long_options[i].val;

        unsigned long algo_id = AlgoTypes.size() - 1;
        arch_id = ArchTypes.size() - 1;

        switch (c) {
            case 0:
                /* Flag is automatically set */
                break;
            case 'h':
                printf("Parallel SGD shared Memory Benchmarks"
                       "\n"
                       "\n"
                       "Usage:\n"
                       "  %s [options...]\n"
                       "\n"
                       "Options:\n"
                       "  -h, --help\n"
                       "        Print this message\n"
                       "  -D, --data-set <int>\n"
                       "        Data set {MNIST, FASHION-MNIST, CIFAR10}\n"
                       "  -b, --batch-size <int>\n"
                       "        Batch size\n"
                       "  -e, --epochs <int>\n"
                       "        Number of epochs\n"
                       "  -r, <int>\n"
                       "        Number of rounds per epochs\n"
                       "  -n, --num-threads <int>\n"
                       "        Number of threads\n"
                       "  -a, --algorithm <string>\n"
                       "        {SEQ, SYNC, ASYNC, HOG, LSH, ELASYNC}\n"
                       "  -A, --architecture <string>\n"
                       "        {MLP, CNN, LENET}\n"
                       "  -L, <int>\n"
                       "        Number of hidden layers\n"
                       "  -U, <int>\n"
                       "        Number of units per hidden layer\n"
                       "  -l, <float>\n"
                       "        Learning rate\n"
                       "  -B, <int>\n"
                       "        CAS backoff threshold, max n.o. failed CAS per step\n"
                       "  -C, <bool>\n"
                       "        In LSH, check for concurrent updates in retry loop\n"
                       "  -t, <bool>\n"
                       "        Staleness-adaptive step size strategy\n"
                       "  -v, --print-vals <int>\n"
                       "        Print debug informations\n"
                       "  -i (elasync only, probing interval)\n"
                       "  -d (elasync only, probing duration)\n"
                       "  -s (elasync only, initial parallelism)\n", argv[0]);
                exit(0);
            case 'a':
                algo_name = optarg;
                while (algo_id >= 0) {
                    if (algo_name == AlgoTypes[algo_id]) {
                        break;
                    }
                    algo_id--;
                }
                run_algo = static_cast<ALGORITHM>(algo_id);
                break;
            case 'A':
                arch_name = optarg;
                while (arch_id >= 0) {
                    if (arch_name == ArchTypes[arch_id]) {
                        break;
                    }
                    arch_id--;
                }
                use_arch = static_cast<ARCHITECTURE>(arch_id);
                break;
            case 'N':
                experiment_name = optarg;
                break;
            case 'b':
                batch_size = atoi(optarg);
                break;
            case 'e':
                num_epochs = atoi(optarg);
                break;
            case 'r':
                rounds_per_epoch = atoi(optarg);
                break;
            case 'B':
                cas_backoff = atoi(optarg);
                break;
            case 'n':
                num_threads = atoi(optarg);
                if (initial_parallelism == -1) initial_parallelism = num_threads/2;
                if (probing_interval == -1) probing_interval = 100 * initial_parallelism;
                if (probing_duration == -1) probing_duration = 10 * initial_parallelism;
                break;
            case 'R':
                run_n = atoi(optarg);
                break;
            case 'l':
                learning_rate = atof(optarg);
                break;
            case 'C':
                check_concurrent_updates = atoi(optarg);
                break;
            case 't':
                tauadaptstrat = optarg;
                break;
            case 'D':
                use_dataset = optarg;
                break;
            case 'L':
                num_hidden_layers = atoi(optarg);
                break;
            case 'U':
                num_hidden_units = atoi(optarg);
                break;
            case 'm':
                momentum_mu = atof(optarg);
                break;
            case 'w':
                probing_window = atoi(optarg);
                break;
            case 'i':
                probing_interval = atoi(optarg); // also sync interval for semisync
                break;
            case 'd':
                probing_duration = atoi(optarg);
                break;
            case 's':
                initial_parallelism = atoi(optarg);
                if (probing_interval == -1) probing_interval = 100 * initial_parallelism;
                if (probing_duration == -1) probing_duration = 10 * initial_parallelism;
                break;
            case 'G':
                scalar_loss_grad = atof(optarg);
                /* heuristic_gradient = atof(optarg); */
                /* std::cout << "Heuristic gradient (G) set to " << heuristic_gradient << std::endl;  */
                break;
            case 'J':
                scalar_loss_jitter = atof(optarg);
                break;
            case 'M':
                scalar_m_trend = atof(optarg);
                break;
            case '?':
            default:
                printf("Use -h or --help for help\n");
                exit(1);
        }
    }

    std::cout << "# Experiment name: " << experiment_name << std::endl;

    // data

    Matrix x;
    Matrix y;

    int in_dim_x;
    int in_dim_y;
    int in_no_chs;

    int num_outputs;

    if (use_dataset == "CIFAR10") {

        in_dim_x = 32;
        in_dim_y = 32;
        in_no_chs = 3;
        num_outputs = 10;

        auto DATASET = cifar::read_dataset<std::vector, std::vector, double, double>();

        std::cout << "# Loaded CIFAR10 dset" << std::endl;
        long n_train = DATASET.training_images.size(); // 50K
        long dim_in = DATASET.training_images[0].size(); // 3072

        typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<double, Eigen::Dynamic, 1> Vector;

        x = Matrix::Zero(dim_in, n_train);

        for (int i = 0; i < n_train; i++)
            x.col(i) = Vector::Map(&DATASET.training_images[i][0], DATASET.training_images[i].size());

        x /= 255; // normalize

        y = Matrix::Zero(10, n_train);

        int T;
        for (int i = 0; i < n_train; i++){
            T = DATASET.training_labels[i];
            if (T < 10 && T >= 0)
                y(T, i) = 1;//Vector::Map(&DATASET.training_labels[i][0], DATASET.training_labels[i].size());
            else
                std::cerr << "# Label value error: " << T << std::endl;
        }
        std::cout << "# Finished loading labels" << std::endl;
    } else if (use_dataset == "CIFAR100") {
        const bool use_fine_labels = true;
        const unsigned n_labels = use_fine_labels ? 100 : 20;

        in_dim_x = 32;
        in_dim_y = 32;
        in_no_chs = 3;
        num_outputs = n_labels;

        cifar::CIFAR100_dataset dset;

        std::cout << "# Reading cifar100 dset" << std::endl;
        cifar::read_cifar100_file(dset.training_images, dset.training_labels, "data/cifar-100/train.bin", use_fine_labels);
        cifar::read_cifar100_file(dset.test_images, dset.test_labels, "data/cifar-100/test.bin", use_fine_labels);

        long n_training = dset.training_images.size();
        long img_n_vals = dset.training_images.at(0).size();
        assert(img_n_vals == 3072);

        typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<double, Eigen::Dynamic, 1> Vector;

        x = Matrix::Zero(img_n_vals, n_training); // One column per training img, one row per pixel

        // Copy data from dataset into matrix
        for (int i = 0; i < n_training; i++) {
            x.col(i) = Vector::Map(&dset.training_images[i][0], dset.training_images[i].size());
        }

        x /= 255;
        y = Matrix::Zero(n_labels, n_training);

        for (int i = 0; i < n_training; i++) {
            int lbl = dset.training_labels.at(i);
            if (lbl < 0 || lbl >= n_labels) {
                std::cerr << "# Label error (" << lbl << ")\n";
            } else {
                y(lbl, i) = 1;
            }
        }
    } else {

        in_dim_x = 28;
        in_dim_y = 28;
        in_no_chs = 1;
        num_outputs = 10;

        std::string data_dir;
        if (use_dataset == "MNIST") {
            data_dir = "./data/mnist/";
        } else if (use_dataset == "FASHION-MNIST") {
            data_dir = "./data/fashion-mnist/";
        }
        MNIST dataset(data_dir);
        dataset.read();

        x = dataset.train_data;
        y = dataset.train_labels;
    }

    // Construct a network object

    auto *init_param = new ParameterContainer(); // variable parameter container pointer

    NetworkTopology network(init_param);

    if (use_arch == ARCHITECTURE::MLP) {
        network.add_layer(new FullyConnected<ReLU>(in_dim_x * in_dim_y * in_no_chs, num_hidden_units));
        for (int i = 0; i < num_hidden_layers - 1; i++) {
            network.add_layer(new FullyConnected<ReLU>(num_hidden_units, num_hidden_units));
        }
        network.add_layer(new FullyConnected<Softmax>(num_hidden_units, num_outputs));
    } else if (use_arch == ARCHITECTURE::CNN) {
        std::cout << "CNN Arch used\n";
        network.add_layer(new Convolutional<ReLU>(28, 28, 1, 4, 3, 3));
        network.add_layer(new MaxPooling<ReLU>(26, 26, 4, 2, 2));

        network.add_layer(new Convolutional<ReLU>(13, 13, 4, 8, 3, 3));
        network.add_layer(new MaxPooling<ReLU>(11, 11, 8, 2, 2));

        network.add_layer(new FullyConnected<ReLU>(5 * 5 * 8, 128));
        network.add_layer(new FullyConnected<Softmax>(128, 10));

    } else if (use_arch == ARCHITECTURE::LENET) {

        if (use_dataset == "CIFAR10" || use_dataset == "CIFAR100") {
            network.add_layer(new Convolutional<ReLU>(32, 32, 3, 6, 5, 5));
            network.add_layer(new MaxPooling<ReLU>(28, 28, 6, 2, 2));

            network.add_layer(new Convolutional<ReLU>(14, 14, 6, 16, 5, 5));
            network.add_layer(new MaxPooling<ReLU>(10, 10, 16, 2, 2));

            network.add_layer(new FullyConnected<ReLU>(5 * 5 * 16, 120));
        } else if (use_dataset == "MNIST" || use_dataset == "FASHION-MNIST") {

            network.add_layer(new Convolutional<ReLU>(28, 28, 1, 6, 5, 5));
            network.add_layer(new MaxPooling<ReLU>(24, 24, 6, 2, 2));

            network.add_layer(new Convolutional<ReLU>(12, 12, 6, 16, 5, 5));
            network.add_layer(new MaxPooling<ReLU>(8, 8, 16, 2, 2));

            network.add_layer(new FullyConnected<ReLU>(4 * 4 * 16, 120));

        }

        network.add_layer(new FullyConnected<Softmax>(120, num_outputs));

    }

    // Set output layer
    network.set_output(new MultiClassEntropy());

    // (Optional) set callback function object
    VerboseCallback callback;
    network.set_callback(callback);

    // Initialize parameters with N(0, 0.01^2) using random seed 123
    network.init(0, 0.01, rand_seed);

    // Create optimizer object
    auto *opt = new SGDM();
    opt->initialize_state(init_param->param_list.size());
    opt->m_lrate = learning_rate;
    opt->mu = momentum_mu;

    std::vector<Optimizer *> thread_local_opts(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
        thread_local_opts[i] = opt->clone();
    }

    int algorithm_id = static_cast<int>(run_algo);
    int architecture_id = static_cast<int>(use_arch);
    NetworkExecutor executor(&network, opt, thread_local_opts, x, y, tauadaptstrat, num_threads, learning_rate, algorithm_id, architecture_id);

    struct timeval start, end;
    gettimeofday(&start, nullptr);

    switch (run_algo) {
        case ALGORITHM::SEQ:
            if (num_threads > 1)
                std::cout << "WARNING: using only 1 thread for sequential optimization" << std::endl;
            executor.run_training(batch_size, num_epochs, rounds_per_epoch, rand_seed);
            break;
        case ALGORITHM::SYNC:
            executor.run_parallel_sync(batch_size, num_epochs, rounds_per_epoch, rand_seed);
            break;
        case ALGORITHM::ASYNC:
            executor.run_parallel_async(batch_size, num_epochs, rounds_per_epoch, start, rand_seed);
            break;
        case ALGORITHM::HOG:
            executor.run_parallel_async(batch_size, num_epochs, rounds_per_epoch, start, rand_seed, false);
            break;
        case ALGORITHM::LSH:
            executor.run_parallel_leashed(batch_size, num_epochs, rounds_per_epoch, cas_backoff, check_concurrent_updates, rand_seed);
            break;
        case ALGORITHM::ELASYNC:
            // executor.scalar_loss_grad = scalar_loss_grad;
            // executor.scalar_loss_jitter = scalar_loss_jitter;
            // executor.scalar_m_trend = scalar_m_trend;
            executor.run_elastic_async(batch_size, num_epochs, rounds_per_epoch, probing_window, probing_interval, probing_duration, initial_parallelism, start, rand_seed, false);
            break;
        case ALGORITHM::SEMISYNC:
            executor.run_semisync(batch_size, num_epochs, rounds_per_epoch, start, probing_interval, rand_seed);
            break;
        case ALGORITHM::HEURISTIC:
            executor.run_heuristic_async(batch_size, num_epochs, rounds_per_epoch, heuristic_gradient, start, rand_seed);
            break;
        default:
            printf("Use -h or --help for help\n");
            exit(1);
            break;
    }

    // if (run_algo == ALGORITHM::ELASYNC) {
    //     SearchController elastic_controller(num_threads, 8);
    //     ModularExecutor prb_exec(&network, opt, x, y, num_threads, learning_rate, 
    //         std::unique_ptr<ParallelismController>(new SearchController(num_threads, 8)));
    //     std::cout << "Executing ProbingExecutor!\n";
    //     prb_exec.run(batch_size, num_epochs, rounds_per_epoch);
    // }

    gettimeofday(&end, nullptr);
    time_t duration;
    duration = end.tv_sec - start.tv_sec;

    Scalar epoch_loss = executor.get_loss();

    jsoncons::json out_json;

    out_json["epoch_loss"] = executor.get_losses_per_epoch();  
    out_json["epoch_time"] = executor.get_times_per_epoch(); 
    out_json["staleness_dist"] = executor.get_tau_dist();
    out_json["numtriesdist"] = executor.get_num_tries_dist();
    
    jsoncons::json mlist;
    mlist["m"] = executor.get_m_values();
    mlist["time"] = executor.get_m_times();
    mlist["probe_starts"] = executor.get_probe_starts(); // Which parallelism values correspond with the first probing step
    mlist["probe_ends"] = executor.get_probe_ends();

    jsoncons::json lossgrad;
    lossgrad["grad"] = executor.get_loss_grads();
    lossgrad["time"] = executor.get_loss_grad_times();

    jsoncons::json meta;
    meta["rand_seed"] = rand_seed;
    meta["learning_rate"] = learning_rate;
    meta["momentum"] = momentum_mu;
    meta["num_threads"] = num_threads;
    meta["batch_size"] = batch_size;
    meta["num_epochs"] = num_epochs;
    meta["run_n"] = run_n;
    meta["probing_duration"] = probing_duration;
    meta["probing_interval"] = probing_interval;
    meta["probing_window"] = probing_window;
    meta["intial_parallelism"] = initial_parallelism;
    meta["dataset"] = use_dataset;
    meta["tauadaptstrat"] = tauadaptstrat;
    meta["arch"] = arch_name;
    meta["algo"] = algo_name;

    out_json["mlist"] = mlist;
    out_json["lossgrad"] = lossgrad;
    out_json["meta"] = meta;
 
    const std::filesystem::path exp_dir = "experiments";
    std::filesystem::create_directory(exp_dir);

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S") << "_" << arch_name << "_" << algo_name << "_" << use_dataset << "_" << experiment_name << ".json";
    std::filesystem::path filepath = exp_dir / ss.str();
    std::ofstream out_file(filepath);
    out_file << out_json;

    std::cout << "# Saved as " << ss.str();

    return 0;
}
