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

    int x_dim, y_dim;
public:
    virtual ~BatchController() = default;
    /* Create controller, load training dataset */
    BatchController(SystemExecutor &exec, int batch_size) : exec(exec) {};

    /* Worker calls this to reserve itself a batch ID. It then uses that ID to
     * get a constant reference to a batch data.
     * This may block, under certain implementations. */
    virtual int get_batch_ind(long worker_id) = 0;

    /* Retrieves the data associated with a batch ID. */
    const Matrix &get_batch_data(int id) { return x_batches.at(id); }
    const Matrix &get_batch_labels(int id) { return y_batches.at(id); }

    int get_x_dimension() const { return this->x_dim; }
    int get_y_dimension() const { return this->y_dim; }

    const std::string &get_dataset_name() { return this->dataset_name; }
};

class ParaController {
protected:
    SystemExecutor &exec;
public:
    virtual ~ParaController() = default;
    ParaController(SystemExecutor &exec) : exec(exec) {}
    virtual unsigned get_parallelism() = 0;

    /* This is called whenever the monitor gets a new update. This allows the dispatcher to update
     * its parallelism in whatever way it wants to then. */
    virtual void update() = 0;
};

class Dispatcher {
protected:
    SystemExecutor &exec;
    long steps_done = 0;
public:
    virtual ~Dispatcher() = default;
    Dispatcher(SystemExecutor &exec) : exec(exec) {}

    virtual bool try_start_step(long worker_id) = 0;
    virtual bool finish_step(long worker_id) = 0;
    virtual bool is_finished() = 0;

    long get_steps_done() const { return steps_done; }
};

class Monitor {
protected:
    SystemExecutor &exec;
public:
    virtual ~Monitor() = default;
    Monitor(SystemExecutor &exec) : exec(exec) {}

    virtual void update(double loss) = 0;

    /* Get a rough but recent estimate of loss */
    virtual double get_loss_estim() = 0;

    /* Compute (maybe blocking) a more accurate estimate of model loss */
    double get_loss_accur() { return this->get_loss_estim(); }
};

class Worker {
protected:
    long id;
    SystemExecutor &exec;
    std::atomic_flag *flag;
public:
    virtual ~Worker() = default;
    Worker(SystemExecutor &exec, long id, std::atomic_flag *flag) : exec(exec), id(id), flag(flag) { }

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
public:
    virtual ~ModelInterface() = default;
    explicit ModelInterface(SystemExecutor &exec) : exec(exec) {}
    ModelInterface(SystemExecutor &exec, NetworkTopology network) : network(network), exec(exec) {}
    std::shared_ptr<NetworkTopology> get_network() {
        return std::make_shared<NetworkTopology>(this->network);
    }

    virtual std::shared_ptr<Optimizer> get_optimizer() = 0;
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
    SystemExecutor(long epoch_target, long steps_per_epoch) : epoch_target(epoch_target), steps_per_epoch(steps_per_epoch) {
        this->start_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now()
            .time_since_epoch()
        ).count();
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

    std::mutex mtx_para_vec;
    std::vector<unsigned> para_values;
    std::vector<long> para_mstimes;

    std::mutex mtx_epoch_vec;
    std::vector<double> epoch_losses;
    std::vector<long> epoch_mstimes;

    std::shared_ptr<BatchController> get_batcher() const { return this->batcher; }
    std::shared_ptr<ParaController> get_paracontr() const { return this->parallelism; }
    std::shared_ptr<Dispatcher> get_dispatcher() const { return this->dispatcher; }
    std::shared_ptr<Monitor> get_monitor() const { return this->monitor; }
    std::shared_ptr<WorkerPool> get_workers() const { return this->workers; }
    std::shared_ptr<ModelInterface> get_model() const { return this->model; }
};

}

#endif /* MODULAR_COMPONENTS_HPP */
