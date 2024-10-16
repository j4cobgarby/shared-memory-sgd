#ifndef MODULAR_COMPONENTS_HPP
#define MODULAR_COMPONENTS_HPP

#include "NetworkTopology.h"
#include "ParameterContainer.h"
#include <memory>
#include <string>
#include <MiniDNN.h>
#include <utils.h> /* Matrix typedef */

namespace MiniDNN {

class SystemExecutor;

class BatchController {
protected:
    SystemExecutor &exec;

    std::vector<Matrix> x_batches;
    std::vector<Matrix> y_batches;
public:
    /* Create controller, load training dataset */
    BatchController(std::string dataset, int batch_size);

    /* Worker calls this to reserve itself a batch ID. It then uses that ID to
     * get a constant reference to a batch data.
     * This may block, under certain implementations. */
    int get_batch_ind(long worker_id);

    /* Retrieves the data associated with a batch ID. */
    const Matrix &get_batch_data(int id) { return x_batches.at(id); }
    const Matrix &get_batch_labels(int id) { return y_batches.at(id); }
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
    Dispatcher(SystemExecutor &exec);

    virtual bool try_start_step(long worker_id) = 0;
    virtual bool finish_step(long worker_id) = 0;
    virtual bool is_finished() = 0;
};

class Monitor {
protected:
    SystemExecutor &exec;
public:
    Monitor(SystemExecutor &exec);

    virtual double update(double loss) = 0;

    virtual double get_loss() = 0;
};

class Worker {
protected:
    long id;
    SystemExecutor &exec;
public:
    Worker(SystemExecutor &exec, long id) : exec(exec), id(id) {}

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
public:
    WorkerPool(SystemExecutor &exec, int n_workers);

    void wait_for_all();
    void start_all();
};

class ModelInterface {
protected:
    /* Volatile because it may, depending on algorithm, be accessed with no
     * locking by threads */
    ParameterContainer param; 

    NetworkTopology network;

    SystemExecutor &exec;
public:
    ModelInterface(SystemExecutor &exec) : exec(exec) {}

    std::shared_ptr<ParameterContainer> get_param() {
        return std::make_shared<ParameterContainer>(this->param);
    }

    std::shared_ptr<NetworkTopology> get_network() {
        return std::make_shared<NetworkTopology>(this->network);
    }
};

class SystemExecutor {
protected:
    std::shared_ptr<BatchController> batcher;
    std::shared_ptr<ParaController> parallelism;
    std::shared_ptr<Dispatcher> dispatcher;
    std::shared_ptr<Monitor> monitor;
    std::shared_ptr<WorkerPool> workers;
    std::shared_ptr<ModelInterface> model;
public:
    void start();

    std::shared_ptr<BatchController> get_batcher() { return this->batcher; }
    std::shared_ptr<ParaController> get_parallelism() { return this->parallelism; }
    std::shared_ptr<Dispatcher> get_dispatcher() { return this->dispatcher; }
    std::shared_ptr<Monitor> get_monitor() { return this->monitor; }
    std::shared_ptr<WorkerPool> get_workers() { return this->workers; }
    std::shared_ptr<ModelInterface> get_model() { return this->model; }
};

}

#endif /* MODULAR_COMPONENTS_HPP */
