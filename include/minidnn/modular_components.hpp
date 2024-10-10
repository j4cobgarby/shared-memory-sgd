#ifndef MODULAR_COMPONENTS_HPP
#define MODULAR_COMPONENTS_HPP

namespace MiniDNN {

class ParallelismController {
public:
    ParallelismController() {}

    virtual int get_m() = 0;

    /* If either of the following two return <0, don't use them as a target */
    virtual long target_phase_steps() = 0; // Max steps in total before threads finish phase
    virtual double target_phase_time() = 0; // Max elapsed time before threads finish phase

    // Tell the controller that the executor finished the current phase, 
    // reporting the loss so that it can work out what to do next.
    virtual void finish_phase(double end_loss) = 0;
};

class BatchController {
public:
    BatchController() {}

    virtual int get_batch_size() = 0;
};

/* A group of workers, maybe threads or some other type.
 * This wraps around a container of all of the workers, and has methods which
 * can handle these workers */
class Workers {
    /* e.g. ThreadPool pool; */
public:
    Workers() {}
};

class Dispatcher {
public:
    Dispatcher() {}

    /* Should the workers stop for one phase? */
    virtual bool worker_should_stop(Workers &group, int id) = 0;

    /* Has the whole algorithm finished? */
    virtual bool is_finished() = 0;
};

}

#endif /* MODULAR_COMPONENTS_HPP */
