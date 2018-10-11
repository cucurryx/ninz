#ifndef  THREAD_SAFE_QUEUE_H

#include "mutex_lock.h"
#include "condition_variable.h"

#include <pthread.h>

#include <memory>
#include <queue>

template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue(): cond_(mutex_) { }

    ThreadSafeQueue(const ThreadSafeQueue &other): cond_(mutex_) {
        MutexLockGuard guard(other.mutex_);
        queue_ = other.queue_;
    }

    ThreadSafeQueue &operator=(const ThreadSafeQueue &queue) = delete;

    void Push(T val);

    bool TryPop(T &val);

    std::shared_ptr<T> TryPop();

    void WaitAndPop(T &val);

    std::shared_ptr<T> WaitAndPop();

    bool Empty() const {
        MutexLockGuard guard(mutex_);
        return queue_.empty();
    }

    size_t Size() const {
        MutexLockGuard guard(mutex_);
        return queue_.size();
    }

private:
    std::queue<T> queue_;
    mutable MutexLock mutex_;
    ConditionVariable cond_;
};


template <typename T>
void ThreadSafeQueue<T>::Push(T val) {
    MutexLockGuard guard(mutex_);
    queue_.push(val);
    cond_.NotifyOne();
}

template <typename T>
bool ThreadSafeQueue<T>::TryPop(T &val) {
    MutexLockGuard guard(mutex_);
    while (queue_.empty()) {
        return false;
    }
    val = queue_.front();
    queue_.pop();
    return true;
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::TryPop() {
    MutexLockGuard guard(mutex_);
    while (queue_.empty()) {
        return std::shared_ptr<T>();
    }
    auto result = std::make_shared<T>(queue_.front());
    queue_.pop();
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::WaitAndPop() {
    MutexLockGuard guard(mutex_);
    while (queue_.empty()) {
        cond_.Wait();
    }
    auto result = std::make_shared<T>(queue_.front());
    queue_.pop();
    return result;
}

template <typename T>
void ThreadSafeQueue<T>::WaitAndPop(T &val) {
    MutexLockGuard guard(mutex_);
    while (queue_.empty()) {
        cond_.Wait();
    }
    val = queue_.front();
    queue_.pop();
}

#endif // ! THREAD_SAFE_QUEUE_H