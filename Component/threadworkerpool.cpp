#include "minidnn/Component/Worker.hpp"
#include "modular_components.hpp"
#include <pthread.h>

namespace MiniDNN {

template <typename WorkerType>
ThreadWorkerPoolAsync<WorkerType>::ThreadWorkerPoolAsync(SystemExecutor &exec, int n_workers, bool pin)
    : WorkerPool(exec, n_workers), loop_sync(n_workers) {
    static_assert(std::is_base_of_v<Worker, WorkerType>,
                  "WorkerType of ThreadWorkerPool _must_ be a subclass of Worker.");

    workers_flag.test_and_set();

    workers.reserve(n_workers);
    worker_threads.reserve(n_workers);

    std::cout << "Creating " << n_workers << " workers.\n";

    for (int i = 0; i < n_workers; i++) {
        workers.push_back(WorkerType(exec, i, &workers_flag));
        // worker_threads.push_back(new std::thread(&WorkerType::run, &workers.at(i)));
        worker_threads.push_back(new std::thread([this, i] {
            this->workers_flag.wait(true); // Wait for flag to lower, for exec start

            while (!this->exec.get_dispatcher()->is_finished()) {
                // while (true)
                this->workers.at(i).run();
                this->loop_sync.arrive_and_wait();
            }
        }));
    }
}

template<typename WorkerType>
void ThreadWorkerPoolAsync<WorkerType>::wait_for_all() {
    std::cout << "Joining all threads.\n";
    for (auto p : worker_threads) {
        if (p->joinable()) {
            p->join();
        }
    }
}

template<typename WorkerType>
void ThreadWorkerPoolAsync<WorkerType>::start_all() {
    workers_flag.clear();
    workers_flag.notify_all();
}

// Instantiate the types of worker we'll use, for the benefit of the linker
template class ThreadWorkerPoolAsync<SGDWorkerAsync>;
template class ThreadWorkerPoolAsync<SGDWorkerSynchronous>;

}
