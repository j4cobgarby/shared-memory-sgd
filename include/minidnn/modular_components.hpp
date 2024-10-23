#ifndef MODULAR_COMPONENTS_HPP
#define MODULAR_COMPONENTS_HPP

#include "NetworkTopology.h"
#include "Optimizer.h"
#include "ParameterContainer.h"
#include <memory>
#include <MiniDNN.h>
#include <utils.h> /* Matrix typedef */

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
    /* Create controller, load training dataset */
    BatchController(SystemExecutor &exec, int batch_size) : exec(exec) {};

    /* Worker calls this to reserve itself a batch ID. It then uses that ID to
     * get a constant reference to a batch data.
     * This may block, under certain implementations. */
    virtual int get_batch_ind(long worker_id) = 0;

    /* Retrieves the data associated with a batch ID. */
    const Matrix &get_batch_data(int id) { return x_batches.at(id); }
    const Matrix &get_batch_labels(int id) { return y_batches.at(id); }

    const int get_x_dimension() { return this->x_dim; }
    const int get_y_dimension() { return this->y_dim; }

    const std::string &get_dataset_name() { return this->dataset_name; }
};

class ParaController {
protected:
    SystemExecutor &exec;
public:
    ParaController(SystemExecutor &exec) : exec(exec) {}
    
    virtual unsigned get_parallelism() = 0;
};

class Dispatcher {
protected:
    SystemExecutor &exec;
public:
    Dispatcher(SystemExecutor &exec) : exec(exec) {}

    virtual bool try_start_step(long worker_id) = 0;
    virtual bool finish_step(long worker_id) = 0;
    virtual bool is_finished() = 0;
};

class Monitor {
protected:
    SystemExecutor &exec;
public:
    Monitor(SystemExecutor &exec) : exec(exec) {}

    virtual void update(double loss) = 0;

    virtual double get_loss() = 0;
};

class Worker {
protected:
    long id;
    SystemExecutor &exec;
    std::atomic_flag *flag;
public:
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
    ModelInterface(SystemExecutor &exec) : exec(exec) {}
    ModelInterface(SystemExecutor &exec, NetworkTopology network) : exec(exec), network(network) {}
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
    SystemExecutor(long epoch_target, long steps_per_epoch) : epoch_target(epoch_target), steps_per_epoch(steps_per_epoch) {}
    
    SystemExecutor(std::shared_ptr<BatchController>,
                   std::shared_ptr<ParaController>,
                   std::shared_ptr<Dispatcher>,
                   std::shared_ptr<Monitor>,
                   std::shared_ptr<WorkerPool>,
                   std::shared_ptr<ModelInterface>,
                   long epoch_target, long steps_per_epoch);

    void set_batcher(std::shared_ptr<BatchController> batcher) { this->batcher = batcher; }
    void set_parallelism(std::shared_ptr<ParaController> parallelism) { this->parallelism = parallelism; }
    void set_dispatcher(std::shared_ptr<Dispatcher> dispatcher) { this->dispatcher = dispatcher; }
    void set_monitor(std::shared_ptr<Monitor> monitor) { this->monitor = monitor; }
    void set_workers(std::shared_ptr<WorkerPool> workers) { this->workers = workers; }
    void set_model(std::shared_ptr<ModelInterface> model) { this->model = model; }

    void start();

    long epoch_target;
    long steps_per_epoch;

    std::shared_ptr<BatchController> get_batcher() { return this->batcher; }
    std::shared_ptr<ParaController> get_paracontr() { return this->parallelism; }
    std::shared_ptr<Dispatcher> get_dispatcher() { return this->dispatcher; }
    std::shared_ptr<Monitor> get_monitor() { return this->monitor; }
    std::shared_ptr<WorkerPool> get_workers() { return this->workers; }
    std::shared_ptr<ModelInterface> get_model() { return this->model; }
};

}

#endif /* MODULAR_COMPONENTS_HPP */
