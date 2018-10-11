#ifndef CONDITION_VARIABLE_H
#define CONDITION_VARIABLE_H

#include "mutex_lock.h"

#include <pthread.h>

class ConditionVariable {
public:
    explicit ConditionVariable(MutexLock &mutex): 
        mutex_(mutex),
        cond_(PTHREAD_COND_INITIALIZER) { }

    void Wait() {
        MutexLockGuard gurad(mutex_);
        pthread_mutex_t m = mutex_.Get();
        CheckReturnVal(pthread_cond_wait(&cond_, &m));
    }

    void NotifyAll() {
        CheckReturnVal(pthread_cond_broadcast(&cond_));
    }

    void NotifyOne() {
        CheckReturnVal(pthread_cond_signal(&cond_));
    }

private:
    MutexLock &mutex_;
    pthread_cond_t cond_;
};

#endif // !CONDITION_VARIABLE_H
