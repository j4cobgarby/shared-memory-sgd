#ifndef EXECUTORS_HPP
#define EXECUTORS_HPP

#include "NetworkExecutor.h"
#include "NetworkTopology.h"
#include "Optimizer.h"
#include "Output/MultiClassEntropy.h"
#include "ParameterContainer.h"
#include "ThreadPool.h"
#include "Utils/Random.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <limits>
#include <mutex>

namespace MiniDNN {

// Provides a HOGWILD!-like non-locking executor which uses probing phases
// to dynamically attempt to find an optimal number of active threads.
class ProbingExecutor : public GenericExecutor {
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::time_point<Clock> Timepoint;
    typedef std::chrono::duration<double> Duration;
protected:

    std::vector<NetworkTopology *> thread_nets;

    int m; // Current parallelism
    int m_0; // Initial parallelism
    long phase_steps; // Total steps to perform in current phase
    long phase_firststep;

    long n_batches; // Number of batches of training data
    
    ParameterContainer *global_param;
    
    std::mutex mtx;
    std::mutex epoch_time_vector_lock;

    std::vector<std::vector<double>> thread_epoch_loss;
    std::vector<std::vector<int>> thread_epoch_rounds;

    std::atomic<int> next_batch   = 0;
    std::atomic<int> step_counter = 0;
    std::atomic_flag threads_stop = ATOMIC_FLAG_INIT;

    Timepoint phase_starttime;
    double phase_maxtime;

    std::unique_ptr<ElasticController> elastic_ctrl;
public:
    ProbingExecutor(NetworkTopology *net, Optimizer *opt, Matrix &x, Matrix &y, int num_threads, double stepsize, std::unique_ptr<ElasticController> elastic_ctrl);

    void thread_work(int id) {
        while (true) {
            if (this->threads_stop.test()) {
                break;
            }

            const long step = this->step_counter.fetch_add(1);

            // Check if we've got to the end of the phase
            if (step - this->phase_firststep > this->phase_steps) {
                this->threads_stop.test_and_set();
                break;
            }

            // Check if we've finished the whole training process
            if (step >= this->num_epochs * this->steps_per_epoch) {
                this->threads_stop.test_and_set();
                break;
            }

            const long batch_index = this->next_batch.fetch_add(1) % this->n_batches;
            const long epoch = step / this->steps_per_epoch;
            const long epoch_step = step % this->steps_per_epoch;

            /* Take a copy of the global model in a lock-free way, HOGWILD!-style */
            auto *local_param = new ParameterContainer(
                *this->global_param
            );

            this->thread_nets[id]->set_pointer(local_param);

            // Do the actual gradient computation.
            this->thread_nets[id]->forward(this->x_batches[batch_index]);
            this->thread_nets[id]->backprop(
                this->x_batches[batch_index],
                this->y_batches[batch_index]
            );

            // This is just the loss measured during the forward pass
            const double loss = this->thread_nets[id]->get_loss();

            this->thread_epoch_loss[id][epoch] += loss;
            this->thread_epoch_rounds[id][epoch]++;

            this->thread_nets[id]->set_pointer(this->global_param);
            this->thread_nets[id]->update_cw(this->thread_local_opts[id]);
            delete local_param;

            if (epoch_step == this->steps_per_epoch - 1) {
                std::cout << "Thread " << id << " finished epoch #" << epoch << "\n";
            }
        }
    }

    void run() {
        this->opt->reset();

        this->n_batches = internal::create_shuffled_batches(x, y, 32, rng, x_batches, y_batches);

        for (int i = 0; i < this->num_threads; i++) {
            this->thread_nets[i] = new NetworkTopology(*net); // Copy network
            this->thread_nets[i]->set_output(
                new MultiClassEntropy(*dynamic_cast<MultiClassEntropy*>(net->get_output()))
            );

            thread_epoch_loss.push_back(std::vector<double>());
            for (int j = 0; j < this->num_epochs; j++) {
                thread_epoch_loss[i].push_back(0);
                thread_epoch_rounds[i].push_back(0);
            }
        }

        this->m = this->m_0 < 0 ? this->num_threads / 2 : this->m_0;

        // Default to epoch = runthrough of all batches
        if (this->steps_per_epoch < 0) this->steps_per_epoch = n_batches;

        this->phase_firststep = this->step_counter;

        this->next_batch = 0;
        this->step_counter = 0;
        this->threads_stop.clear();

        this->global_param = this->net->current_param_container_ptr;

        std::vector<std::function<void(int id)>> jobs;
        for (int i = 0; i < this->num_threads; i++) {
            jobs.push_back(
                std::bind(
                    &ProbingExecutor::thread_work, this, std::placeholders::_1
                )
            );
        }

        ThreadPool workers(this->num_threads, jobs);
        workers.wait_for_all();

        double prev_loss = -1;
        bool prev_loss_set = false;

        long curr_step;

        while ((curr_step = this->step_counter.load()) < this->num_epochs * this->steps_per_epoch) {
            this->m = this->elastic_ctrl->get_m();
            this->phase_steps = this->elastic_ctrl->target_phase_steps();
            this->phase_maxtime = this->elastic_ctrl->target_phase_time();

            // Sanity check for wonky parallelism
            if (this->m > this->num_threads || this->m <= 0) {
                elastic_ctrl->finish_phase(std::numeric_limits<double>::infinity());
                continue;
            }

            threads_stop.clear();
            
            const Timepoint work_start = Clock::now();

            workers.start_all();
            workers.wait_for_all();

            const Timepoint work_end = Clock::now();
            const Duration work_dur = work_end - work_start;
            const double work_seconds = work_dur.count();

            std::cout << "<executor> last phase took " << work_seconds << " seconds\n";

            double loss = 0;
            unsigned int contributors = 0;
            
            for (int i = 0; i < this->m; i++) {
                if (!std::isnan(this->thread_nets[i]->get_loss())) {
                    contributors++;
                    loss += thread_nets[i]->get_loss();
                }
            }

            elastic_ctrl->finish_phase(loss / contributors);
        }
    }
};

}

#endif /* EXECUTORS_HPP */
