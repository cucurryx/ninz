#include "thread.h"

#include <assert.h>

#include <memory>
#include <iostream>

struct ThreadFunctionWrapper {
    std::function<void()> thread_function;
};

void *ThreadStartRoutine(void *arg) {
    auto func_ptr = static_cast<ThreadFunctionWrapper*>(arg);
    //log: start
    func_ptr->thread_function();
    delete func_ptr;
    return NULL;
}

Thread::Thread(const ThreadFunction &func):
    joined_(false),
    thread_id_(0),
    thread_function_(func) { }

Thread::Thread(ThreadFunction &&func):
    joined_(false),
    thread_id_(0),
    thread_function_(func) { }

void Thread::Join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    int retval = pthread_join(thread_id_, NULL);
    if (retval != 0) {
        assert(false);
        //log error
    }
}

void Thread::Detach() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    int retval = pthread_detach(thread_id_);
    if (retval != 0) {
        assert(false);
        //log error
    }
}

void Thread::Start() {
    assert(!started_);
    started_ = true;
    auto wrapper_ptr = new ThreadFunctionWrapper;
    wrapper_ptr->thread_function = thread_function_;

    int retval = pthread_create(&thread_id_, NULL, ThreadStartRoutine, static_cast<void*>(wrapper_ptr));
    if (retval != 0) {
        delete wrapper_ptr;
        assert(false);
        //log error
    }
}