// created by xvvx on 2018-9-30

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

#include <functional>

class Thread {
public:
    using ThreadFunction = std::function<void()>;

public:
    explicit Thread(const ThreadFunction &func);
    explicit Thread(ThreadFunction &&func);

    Thread &operator=(const Thread &) = delete;
    Thread &operator=(Thread &&) = delete;
    
    ~Thread() {
        if (started_ && !joined_) {
            Detach();
        }
    }

    void Join();

    bool Joinable() {
        return started_ && !joined_;
    }

    pthread_t GetThreadId() {
        return thread_id_;
    }

    void Start();

private:
    void Detach();

private:
    bool joined_;
    bool started_;
    pthread_t thread_id_;
    ThreadFunction thread_function_;
};



#endif // !THREAD_H
