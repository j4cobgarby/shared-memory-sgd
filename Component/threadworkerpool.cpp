#include "minidnn/Component/Worker.hpp"
#include "modular_components.hpp"
#include <pthread.h>

namespace MiniDNN {

template <typename WorkerType>
ThreadWorkerPool<WorkerType>::ThreadWorkerPool(SystemExecutor &exec, int n_workers, bool pin)
    : WorkerPool(exec, n_workers)
{
    static_assert(std::is_base_of<Worker, WorkerType>::value, "WorkerType of ThreadWorkerPool _must_ be a subclass of Worker.");

    workers_flag.test_and_set();

    workers.reserve(n_workers);
    worker_threads.reserve(n_workers);

    std::cout << "Creating " << n_workers << " workers.\n";

    for (int i = 0; i < n_workers; i++) {
        workers.push_back(WorkerType(exec, i, &workers_flag));
        worker_threads.push_back(new std::thread(&WorkerType::run, &workers.at(i)));

        if (pin) {
            // cpu_set_t cpuset;
            // CPU_ZERO(&cpuset);
            // CPU_SET(i, &cpuset);
            //
            // if (pthread_setaffinity_np(worker_threads.at(i)->native_handle(), sizeof(cpu_set_t), &cpuset) != 0) {
            //     std::cerr << "Failed to pin thread " << i << std::endl;
            // }
        }
    }

    std::cout << "Created all workers.\n";
}

template<typename WorkerType>
void ThreadWorkerPool<WorkerType>::wait_for_all() {
    for (auto p : worker_threads) {
        if (p->joinable()) {
            p->join();
        }
    }
}

template<typename WorkerType>
void ThreadWorkerPool<WorkerType>::start_all() {
    workers_flag.clear();
    workers_flag.notify_all();
}

template class ThreadWorkerPool<SGDWorker>;

}
