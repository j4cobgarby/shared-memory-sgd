#ifndef EXECUTORS_HPP
#define EXECUTORS_HPP

#include "NetworkExecutor.h"
#include "NetworkTopology.h"
#include "Output/MultiClassEntropy.h"
#include "ThreadPool.h"
#include "Utils/Random.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>

namespace MiniDNN {

class ElasticController {
public:
    ElasticController();

    int get_m();
    long get_steps();

    bool is_probing();

    // Tell the controller that the executor finished the current phase, 
    // reporting the loss so that it can work out what to do next.
    void finish_phase(double end_loss);
};

class ProbingExecutor : public GenericExecutor {
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::time_point<Clock> Timepoint;
    typedef std::chrono::duration<double> Duration;
protected:

    std::vector<NetworkTopology *> thread_nets;

    int m; // Current parallelism
    int m_0; // Initial parallelism
    long phase_steps; // Total steps to perform in current phase
    
    std::mutex mtx;
    std::mutex epoch_time_vector_lock;

    std::vector<std::vector<double>> thread_epoch_loss;
    std::vector<std::vector<int>> thread_epoch_rounds;

    std::atomic<int> next_batch;
    std::atomic<int> step;
    std::atomic_flag threads_stop = ATOMIC_FLAG_INIT;

    Timepoint phase_starttime;

    ElasticController elastic;
public:
    void thread_work(int id) {
        while (true) {
            // ...
        }
    }
    void run() {
        this->opt->reset();

        const int n_batches = internal::create_shuffled_batches(x, y, 32, rng, x_batches, y_batches);

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
        if (this->rounds_per_epoch < 0) this->rounds_per_epoch = n_batches;

        this->next_batch = 0;
        this->step = 0;
        this->threads_stop.clear();

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

        long curr_step;
        while ((curr_step = this->step.load()) < this->num_epochs * this->rounds_per_epoch) {
            this->m = this->elastic.get_m();
            this->phase_steps = this->elastic.get_steps();

            if (this->m > this->num_threads || this->m <= 0) {
                continue;
            }

            threads_stop.clear();
            
            const Timepoint work_start = Clock::now();

            workers.start_all();
            workers.wait_for_all();

            const Timepoint work_end = Clock::now();
            const Duration work_dur = work_end - work_start;
            const double work_seconds = work_dur.count();

            std::cout << "executor: phase took " << work_seconds << " seconds\n";

            double loss = 0;
            int contributors = 0;
            
            for (int i = 0; i < this->m; i++) {
                if (!std::isnan(this->thread_nets[i]->get_loss())) {
                    contributors++;
                    loss += thread_nets[i]->get_loss();
                }
            }
            loss /= contributors;

        }
    }
};

}

#endif /* EXECUTORS_HPP */
