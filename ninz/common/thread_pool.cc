//
// Created by xvvx on 18-10-2.
//
#include "thread_pool.h"

#include <thread>
#include <iostream>


ThreadPool::ThreadPool(size_t num_thread,
                       size_t max_queue_size):
        mutex_(),
        not_empty_(mutex_),
        not_full_(mutex_),
        max_queue_size_(max_queue_size),
        running_(false) {
    assert(max_queue_size > 0);
    for (int i = 0; i < num_thread; ++i) {
        threads_.emplace_back(new Thread([this](){ this->WorkerThread(); }));
    }
    //log
}

ThreadPool::~ThreadPool() {
    if (running_) {
        Stop();
    }
}

void ThreadPool::Start() {
    assert(!running_);
    running_ = true;
    for (const auto &t : threads_) {
        t->Start();
    }
    // log
}

void ThreadPool::Stop() {
    mutex_.Lock();

    assert(running_);
    running_ = false;

    not_empty_.NotifyAll(); //wake up all sleeping threads
    mutex_.Unlock();

    for (const auto &t : threads_) {
        t->Join();
    }
}

size_t ThreadPool::GetQueueSize() const {
    return max_queue_size_;
}

void ThreadPool::Submit(const ThreadPool::Task &task) {
    if (threads_.empty()) {
        task();
    } else {
        MutexLockGuard guard(mutex_);
        while (task_queue_.size() >= max_queue_size_) {
            not_full_.Wait();
        }
        assert(!IsFull());
        task_queue_.push_back(task);
        not_empty_.NotifyOne();
    }
}

void ThreadPool::WorkerThread() {
    try {
        while (running_) {
            Task task(Take());
            if (task) {
                task();
            }
        }
    } catch (...) {
        //LOG
    }
}

// if running_ == false, not wait
ThreadPool::Task ThreadPool::Take() {
    MutexLockGuard guard(mutex_);
    while (task_queue_.empty() && running_) {
        not_empty_.Wait();
    }

    Task result;
    if (!task_queue_.empty()) {
        result = task_queue_.front();
        task_queue_.pop_front();
        not_full_.NotifyOne();
    }
    return result;
}

bool ThreadPool::IsFull() const {
    return task_queue_.size() >= max_queue_size_;
}
