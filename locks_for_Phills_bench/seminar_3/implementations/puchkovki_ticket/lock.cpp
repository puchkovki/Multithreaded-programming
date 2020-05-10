#include "lock.h"
#include <string>
#include <atomic>
#include <iostream>
#include <unistd.h>
#include <thread> 
#include <cstdlib>
#include <cassert>

//Using a comma operator provided it has not been overloaded
#define assertm(exp, msg) assert(((void)msg, exp))

struct lock {
private:
    std::atomic_size_t now_serving = {0};
    std::atomic_size_t next_ticket = {0};

    // Number of threads
    unsigned int number_of_threads;

public:
    lock(unsigned int num_thr): now_serving(0), next_ticket(0), number_of_threads(num_thr) {}
    // Standard destructor
    // In case if lock was not released beforehand, writes an error
    ~lock() { assertm((next_ticket > number_of_threads), "Had happened some mistake!"); }

    // Try to acquire the spinlock
    void acquire() {
        /*// Initialize time for sleeping
        unsigned int timer = 1;*/
        
        const auto ticket = next_ticket.fetch_add(1, std::memory_order_relaxed);
        while (now_serving.load(std::memory_order_acquire) != ticket) {
            /*if(number_of_threads < sysconf(_SC_NPROCESSORS_ONLN) * 10) {
                    sleep(timer);

                    // Double timer
                    timer <<= 1;
                } else {*/
                    // yield operation
                    __asm volatile ("pause" ::: "memory");
                /*}*/
        };        
    }

    // Try to release the spinlock
    void release() {
        const auto successor = now_serving.load(std::memory_order_acquire) + 1;
        now_serving.store(successor, std::memory_order_release);
    }
};

/*
 * Allocates resources for the lock.
 *
 * n_threads is number of threads that will
 * share this lock. lock_alloc() may ignore
 * this argument.
 *
 * It is guaranteed to be called only once from a
 * calling program, so allocation in static memory is OK.
 *
 * Returns pointer to the lock in case
 * of success and (void*)(NULL) otherwise.
 */
lock_t* lock_alloc(long unsigned n_threads) {
    lock_t *ticket_lock = new lock_t(n_threads);
    if(ticket_lock == NULL) {
        return NULL;
    }
    return ticket_lock;
}

/*
 * Acquires the lock.
 *
 * 'arg' is the value returned by lock_alloc() function.
 *
 * Returns zero in case of success and nonzero value
 * otherwise, e.g. if lock is in inconsistent state.
 */
int lock_acquire(lock_t* arg) {
    if(arg == NULL) {
        return EXIT_FAILURE;
    }
    arg->acquire();
    return EXIT_SUCCESS;
}

/*
 * Releases the lock.
 *
 * 'arg' is the value returned by lock_alloc() function.
 *
 * Returns zero in case of success and nonzero value
 * otherwise, e.g. if lock was not acquired by the caller.
 */
int lock_release(lock_t* arg) {
    if(arg == NULL) {
        return EXIT_FAILURE;
    }
    arg->release();
    return EXIT_SUCCESS;
}

/*
 * Frees resources associated with the lock.
 *
 * 'arg' is the value returned by lock_alloc() function.
 *
 * It is guaranteed to be called only once for the
 * successfully alloc()-ed lock.
 *
 * Returns zero in case of success and nonzero value
 * otherwise, e.g. if lock was not released beforehand.
 */
int lock_free(lock_t* arg) {
    delete arg;
    return EXIT_SUCCESS;
}