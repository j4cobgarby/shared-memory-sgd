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

namespace MiniDNN {

class SystemExecutor;

class BatchController {
protected:
    SystemExecutor &exec;

    std::vector<Matrix> x_batches;
    std::vector<Matrix> y_batches;

    std::string dataset_name;

    // y_dim: Dimension of output, i.e. number of categories
    // x_dim: Dimension of input. For img, it's rows * cols * channels
    int x_dim = 0, y_dim = 0;
public:
    virtual ~BatchController() = default;
    BatchController(SystemExecutor &exec) : exec(exec) {}

    /* Worker calls this to reserve itself a batch ID. It then uses that ID to
     * get a constant reference to a batch data.
     * This may block, under certain implementations. */
    virtual unsigned long get_batch_ind(long worker_id, std::unique_ptr<int> batch_size_out) = 0;

    /* Retrieves the data associated with a batch ID. */
    virtual Matrix get_batch_data(const unsigned long id, int batch_size) { return x_batches.at(id); }
    virtual Matrix get_batch_labels(const unsigned long id, int batch_size) { return y_batches.at(id); }

    virtual int get_x_dimension() const { return this->x_dim; }
    virtual int get_y_dimension() const { return this->y_dim; }

    const std::string &get_dataset_name() { return this->dataset_name; }
};

class ParaController {
protected:
    SystemExecutor &exec;
    std::mutex mtx;
public:
    virtual ~ParaController() = default;
    ParaController(SystemExecutor &exec) : exec(exec) {}
    virtual unsigned get_parallelism() = 0;
    virtual unsigned get_latest_exec_parallelism() = 0;

    /* This is called whenever the monitor gets a new update. This allows the dispatcher to update
     * its parallelism in whatever way it wants to then. */
    virtual void update(long step) = 0;
};

class Dispatcher {
protected:
    SystemExecutor &exec;
    std::atomic<long> steps_done = 0;
public:
    virtual ~Dispatcher() = default;
    Dispatcher(SystemExecutor &exec) : exec(exec) {}

    virtual bool try_start_step(long worker_id) = 0;
    virtual long finish_step(long worker_id) = 0;
    virtual bool is_finished() = 0;

    long get_steps_done() const { return steps_done; }
};

class Monitor {
protected:
    SystemExecutor &exec;
public:
    virtual ~Monitor() = default;
    Monitor(SystemExecutor &exec) : exec(exec) {}

    virtual void update(double loss, long duration_ns, long step) = 0;

    /* Get a rough estimate of absolute loss, based on SGD training evaluations */
    virtual double get_loss_estim() = 0;

    /* Get a rough estimate of delta loss per step */
    virtual double get_rate_estim() = 0;

    /* Compute (maybe blocking) a more accurate estimate of absolute model loss */
    virtual double get_loss_accur() = 0;
};

class Worker {
protected:
    long id;
    SystemExecutor &exec;
    std::atomic_flag *flag;
public:
    virtual ~Worker() = default;
    Worker(SystemExecutor &exec, const long id, std::atomic_flag *flag) : id(id), exec(exec), flag(flag) { }

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
    SystemExecutor &exec;
public:
    virtual ~WorkerPool() = default;
    WorkerPool(SystemExecutor &exec, int n_workers) : exec(exec) {};

    virtual void wait_for_all() = 0;
    virtual void start_all() = 0;
};

/* This base ModelInterface implements a simple interface to a LeNet type
 * network. */
class ModelInterface {
protected:
    NetworkTopology network;

    SystemExecutor &exec;

    std::vector<std::vector<double>> saved_params;
public:
    virtual ~ModelInterface() = default;
    explicit ModelInterface(SystemExecutor &exec) : exec(exec) {}
    ModelInterface(SystemExecutor &exec, NetworkTopology network) : network(network), exec(exec) {}
    std::shared_ptr<NetworkTopology> get_network() {
        return std::make_shared<NetworkTopology>(this->network);
    }

    virtual std::shared_ptr<Optimizer> get_optimizer() = 0;

    void save_network_params(std::vector<std::vector<double>> &p) {p = this->network.get_parameters();}
    void save_network_params() {saved_params = this->network.get_parameters();}

    void load_network_params(const std::vector<std::vector<double>> &p) {this->network.set_parameters(p);}
    void load_network_params() {this->network.set_parameters(saved_params);}
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
    SystemExecutor(const long epoch_target, const long steps_per_epoch) : epoch_target(epoch_target),
                                                                          steps_per_epoch(steps_per_epoch) {
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

    long epoch_target;
    long steps_per_epoch;

    long start_time_ms = -1;
    HRClock::time_point start_time_hr;

    std::mutex mtx_para_vec;
    std::vector<unsigned> para_values;
    std::vector<long> para_mstimes;
    std::vector<bool> para_is_probing;

    std::mutex mtx_epoch_vec;
    std::vector<double> epoch_losses;
    std::vector<long> epoch_mstimes;

    std::mutex mtx_steptime_samples;
    std::vector<std::tuple<long, long>> steptime_samples;

    long submit_para_change(long m, bool is_probing);
    void submit_steptimes(std::vector<std::tuple<long, long>>&);

    std::shared_ptr<BatchController> get_batcher() const { return this->batcher; }
    std::shared_ptr<ParaController> get_paracontr() const { return this->parallelism; }
    std::shared_ptr<Dispatcher> get_dispatcher() const { return this->dispatcher; }
    std::shared_ptr<Monitor> get_monitor() const { return this->monitor; }
    std::shared_ptr<WorkerPool> get_workers() const { return this->workers; }
    std::shared_ptr<ModelInterface> get_model() const { return this->model; }
};

}

#endif /* MODULAR_COMPONENTS_HPP */
