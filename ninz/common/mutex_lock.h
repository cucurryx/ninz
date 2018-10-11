// created by xvvx on 2018-10-01

#ifndef MUTEX_LOCK_H
#define MUTEX_LOCK_H

#include <assert.h>
#include <pthread.h>

inline void CheckReturnVal(int retval) {
    assert(retval == 0);
}

class MutexLock {
public:
    MutexLock(): mutex_(PTHREAD_MUTEX_INITIALIZER) { }

    ~MutexLock() { }

    void Lock() {
        CheckReturnVal(pthread_mutex_lock(&mutex_));
    }

    void Unlock() {
        CheckReturnVal(pthread_mutex_unlock(&mutex_));
    }

    pthread_mutex_t Get() {
        return mutex_;
    }

private:
    pthread_mutex_t mutex_;
};

class MutexLockGuard {
public:
    MutexLockGuard(MutexLock &mutex):
        mutex_(mutex) {
        mutex_.Lock();
    }

    ~MutexLockGuard() {
        mutex_.Unlock();
    }

private:
    MutexLock &mutex_;
};


#endif // M