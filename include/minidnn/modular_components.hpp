#ifndef MODULAR_COMPONENTS_HPP
#define MODULAR_COMPONENTS_HPP

#include "NetworkTopology.h"
#include "Optimizer.h"
#include "ParameterContainer.h"
#include <memory>
#include <utility>
#include <MiniDNN.h>
#include <utils.h> /* Matrix typedef */
#include <atomic>

#define MAX_TAU_DIST 512
#define MAX_MEASURE_TAU_PER_EPOCH 256

namespace MiniDNN {

class SystemExecutor;

class BatchController {
protected:
    SystemExecutor &_exec;

    std::vector<Matrix> _x_batches;
    std::vector<Matrix> _y_batches;


    std::string _dataset_name;

    // y_dim: Dimension of output, i.e. number of categories
    // x_dim: Dimension of input. For img, it's rows * cols * channels
    int _x_dim = 0, _y_dim = 0;
public:
    virtual ~BatchController() = default;
    BatchController(SystemExecutor &exec) : _exec(exec) {}

    /* Worker calls this to reserve itself a batch ID. It then uses that ID to
     * get a constant reference to a batch data.
     * This may block, under certain implementations. */
    virtual unsigned long get_batch_ind(long worker_id, std::unique_ptr<int> batch_size_out) = 0;

    /* Retrieves the data associated with a batch ID. */
    virtual Matrix get_batch_data(const unsigned long id, int batch_size) { return _x_batches.at(id); }
    virtual Matrix get_batch_labels(const unsigned long id, int batch_size) { return _y_batches.at(id); }

    virtual int get_x_dimension() const { return this->_x_dim; }
    virtual int get_y_dimension() const { return this->_y_dim; }

    const std::string &get_dataset_name() { return this->_dataset_name; }

    Matrix _test_x, _test_y;
    Matrix _train_x, _train_y;
};

class ParaController {
protected:
    SystemExecutor &_exec;
    std::mutex _mtx;
public:
    virtual ~ParaController() = default;
    ParaController(SystemExecutor &exec) : _exec(exec) {}
    virtual unsigned get_parallelism() = 0;
    virtual unsigned get_latest_exec_parallelism() = 0;

    /* This is called whenever the monitor gets a new update. This allows the dispatcher to update
     * its parallelism in whatever way it wants to then. */
    virtual void update(long step) = 0;
};

class Dispatcher {
protected:
    SystemExecutor &_exec;
    std::atomic<long> _steps_done = 0;
    std::atomic<long> _steps_started = 0;


public:
    virtual ~Dispatcher() = default;
    Dispatcher(SystemExecutor &exec) : _exec(exec) {}

    virtual std::pair<bool, long> try_start_step(long worker_id) = 0;
    virtual bool finish_step(long worker_id, long step_ind, long &end_step_out) = 0;
    virtual bool is_finished() = 0;

    long get_steps_done() const { return _steps_done; }
};

class Monitor {
protected:
    SystemExecutor &_exec;
public:
    virtual ~Monitor() = default;
    Monitor(SystemExecutor &exec) : _exec(exec) {}

    virtual void update(double loss, long duration_ns, long step) = 0;

    /* Get a rough estimate of absolute loss, based on SGD training evaluations */
    virtual double get_loss_estim() = 0;

    /* Get a rough estimate of delta loss per step */
    virtual double get_rate_estim() = 0;

    /* Compute (maybe blocking) a more accurate estimate of absolute model loss */
    virtual double get_loss_accur() = 0;

    virtual double eval_accuracy(bool training_set = true);
};

class Worker {
protected:
    long _id;
    SystemExecutor &_exec;
    std::atomic_flag *_flag;
public:
    virtual ~Worker() = default;
    Worker(SystemExecutor &exec, const long id, std::atomic_flag *flag) : _id(id), _exec(exec), _flag(flag) { }

    /* For a thread, this can be the actual function to run. For a distributed
     * node it would do something like sending a message to the correct node. */
    virtual void run() = 0;
};

/* WorkerPool
 * ---
 * This class represents a runnable pool of workers. These can be shared-memory
 * threads, distributed nodes, or anything else.
 * This design implies that the system as a whole is centralised: all of the
 * workers are invoked by this class, which is controlled by the dispatcher.
 */
class WorkerPool {
protected:
    SystemExecutor &_exec;
public:
    virtual ~WorkerPool() = default;
    WorkerPool(SystemExecutor &exec, int n_workers) : _exec(exec) {};

    virtual void wait_for_all() = 0;
    virtual void start_all() = 0;
};

/* This base ModelInterface implements a simple interface to a LeNet type
 * network. */
class ModelInterface {
protected:
    NetworkTopology _network;

    SystemExecutor &_exec;

    std::vector<std::vector<double>> _saved_params;
public:
    virtual ~ModelInterface() = default;
    explicit ModelInterface(SystemExecutor &exec) : _exec(exec) {}
    ModelInterface(SystemExecutor &exec, NetworkTopology network) : _network(network), _exec(exec) {}
    std::shared_ptr<NetworkTopology> get_network() {
        return std::make_shared<NetworkTopology>(this->_network);
    }

    virtual std::shared_ptr<Optimizer> get_optimizer() = 0;

    void save_network_params(std::vector<std::vector<double>> &p) {p = this->_network.get_parameters();}
    void save_network_params() {_saved_params = this->_network.get_parameters();}

    void load_network_params(const std::vector<std::vector<double>> &p) {this->_network.set_parameters(p);}
    void load_network_params() {this->_network.set_parameters(_saved_params);}
};

class SystemExecutor {
protected:
    std::shared_ptr<BatchController> batcher = nullptr;
    std::shared_ptr<ParaController> parallelism = nullptr;
    std::shared_ptr<Dispatcher> dispatcher = nullptr;
    std::shared_ptr<Monitor> monitor = nullptr;
    std::shared_ptr<WorkerPool> workers = nullptr;
    std::shared_ptr<ModelInterface> model = nullptr;
public:
    SystemExecutor(const long epoch_target, const long steps_per_epoch) : _epoch_target(epoch_target),
                                                                          _steps_per_epoch(steps_per_epoch) {
        this->start_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now()
            .time_since_epoch()
        ).count();

        this->start_time_hr = HRClock::now();
    }
    
    SystemExecutor(std::shared_ptr<BatchController>,
                   std::shared_ptr<ParaController>,
                   std::shared_ptr<Dispatcher>,
                   std::shared_ptr<Monitor>,
                   std::shared_ptr<WorkerPool>,
                   std::shared_ptr<ModelInterface>,
                   long epoch_target, long steps_per_epoch);

    void set_batcher(std::shared_ptr<BatchController> batcher)
        { this->batcher = std::move(batcher); }

    void set_parallelism(std::shared_ptr<ParaController> parallelism)
        { this->parallelism = std::move(parallelism); }

    void set_dispatcher(std::shared_ptr<Dispatcher> dispatcher)
        { this->dispatcher = std::move(dispatcher); }

    void set_monitor(std::shared_ptr<Monitor> monitor)
        { this->monitor = std::move(monitor); }

    void set_workers(std::shared_ptr<WorkerPool> workers)
        { this->workers = std::move(workers); }

    void set_model(std::shared_ptr<ModelInterface> model)
        { this->model = std::move(model); }

    void start();
    long elapsed_time() const;

    long _epoch_target;
    long _steps_per_epoch;

    double first_loss;
    bool got_first_loss = false;

    long start_time_ms = -1;
    HRClock::time_point start_time_hr;

    std::mutex mtx_para_vec;
    std::vector<unsigned> _para_values;
    std::vector<long> _para_mstimes;
    std::vector<bool> para_is_probing;
    std::vector<long> _para_stepinds;

    std::mutex mtx_epoch_vec;
    std::vector<double> _epoch_losses;
    std::vector<long> _epoch_mstimes;

    std::mutex mtx_async_period_vec;
    std::vector<double> _async_period_values;
    std::vector<long> _async_period_mstimes;

    std::mutex mtx_steptime_samples;
    std::vector<std::tuple<long, long, long, long>> _steptime_samples;

    std::mutex _mtx_tau_dist;
    std::array<long, MAX_TAU_DIST> _tau_dist = {0};

    std::mutex _mtx_epoch_tau_dist;
    std::vector<std::array<long, MAX_MEASURE_TAU_PER_EPOCH>> _epoch_tau_dist;

    std::mutex _mtx_accept_rate;
    long _accepted_steps = 0, _rejected_steps = 0;

    long submit_para_change(long m, bool is_probing);
    void submit_steptimes(std::vector<std::tuple<long, long, long, long>> &);
    void submit_tau_dist(const std::array<long, MAX_TAU_DIST> &);
    void submit_epoch_tau_dist(const std::vector<std::array<long, MAX_MEASURE_TAU_PER_EPOCH>> &);
    void submit_acceptance_rate(const long accepted, const long rejected);

    std::shared_ptr<BatchController> get_batcher() const { return this->batcher; }
    std::shared_ptr<ParaController> get_paracontr() const { return this->parallelism; }
    std::shared_ptr<Dispatcher> get_dispatcher() const { return this->dispatcher; }
    std::shared_ptr<Monitor> get_monitor() const { return this->monitor; }
    std::shared_ptr<WorkerPool> get_workers() const { return this->workers; }
    std::shared_ptr<ModelInterface> get_model() const { return this->model; }
};

}

#endif /* MODULAR_COMPONENTS_HPP */
