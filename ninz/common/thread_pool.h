//
// Created by xvvx on 18-10-2.
//

#ifndef BASE_TOOLS_THREAD_POOL_H
#define BASE_TOOLS_THREAD_POOL_H

#include "thread_safe_queue.h"
#include "thread.h"

#include <functional>
#include <atomic>

class ThreadPool {
public:
    using Task = std::function<void(void)>;
    using ThreadPtr = std::shared_ptr<Thread>;
    using ThreadList = std::vector<ThreadPtr>;

public:
    explicit ThreadPool(size_t num_thread,
                        size_t max_queue_size);

    ~ThreadPool();

    void Start();

    void Stop();

    size_t GetQueueSize() const;

    void Submit(const Task &task);

private:
    void WorkerThread();

    Task Take();

    bool IsFull() const;

private:
    MutexLock mutex_;
    ConditionVariable not_empty_;
    ConditionVariable not_full_;
    ThreadList threads_;
    std::deque<Task> task_queue_;
    std::atomic_bool running_;
    const size_t max_queue_size_;
};


#endif //BASE_TOOLS_THREAD_POOL_H
