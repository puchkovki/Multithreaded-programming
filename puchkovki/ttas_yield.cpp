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
    std::atomic<unsigned int> m_spin;

    // Number of threads
    unsigned int number_of_threads;

public:
    lock(unsigned int num_thr): m_spin(0), number_of_threads(num_thr) {}
    // Standard destructor
    // In case if lock was not released beforehand, writes an error
    ~lock() { assertm((m_spin.load(std::memory_order_acquire) == 0), "Lock in not unlocked yet!"); }

    // Try to acquire the spinlock
    void acquire() {
        unsigned int expected;
        do {
            /*// Initialize time for sleeping
            unsigned int timer = 1;*/

            // While spinlock is locked (m_spin == 1)
            while (m_spin.load(std::memory_order_acquire)) {
                // If there are not so many threads in queue, 
                // so context change after yield operation will last too long, 
                // sleep for an exponential time
                /*if(number_of_threads > sysconf(_SC_NPROCESSORS_ONLN) * 50) {
                    sleep(timer);

                    // Double timer
                    timer <<= 1;
                } else {*/
                    // yield operation
                    __asm volatile ("pause" ::: "memory");
                /*}*/
            };
            expected = 0;
        }
        while ( !m_spin.compare_exchange_weak(expected, 1));
    }

    // Try to release the spinlock
    void release() {
        m_spin.store(0, std::memory_order_release);
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
    lock_t *spin_lock = new lock_t(n_threads);
    if(spin_lock == NULL) {
        return NULL;
    }
    return spin_lock;
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