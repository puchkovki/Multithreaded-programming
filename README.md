# **Multithreaded programming DATI**
Multithreaded programming class for the DCAM MIPT students in Acronis.

## **[pthread_id](pthread_id/)**

Write program receiving input number `N` as an command line argument and making
`N` threads via `pthreads` or `C+11 threads` library. Each thread must print its
executable number in order and its ID.

## **[pthread_wr](pthread_wr/)**

Write program producing 4 threads, which fill shared buffer with 100 characters 
`'1'`, `'2'`, `'3'` and `'4'` correspondingly, using `pthreads` or 
`C+11 threads` library. Threads stop after writing 100 characters, and other 4 
threads read 100 characters from the buffer and put them into 4 different data 
files.

## **[exec_ls](exec_ls/)**

Write running program executing `ls` in a child process.

## **Locks**

Diefferent realisations of `std::atomic` locks:
 - Test and test-and-set lock (TTAS)
   - yield
   - exponential backoff (sleep)
 - Ticket lock
   - yield
   - exponential backoff (sleep)

Realization checks on two benchmarks: *[Phillip's](./locks_for_Phills_bench)*
and *[Vsevolod's](./locks_for_Seva_bench)*.


## **[matrix](matrix/)**

Realisation of the multithreading block matrix's multiplication. Checked for the 
cache misses with `valgrind`.

## **[Stack](multithreadedList/)**

Realisation of the multithreaded linked list with solution of the multithreaded [ABA problem](https://en.wikipedia.org/wiki/ABA_problem).
Checked with the [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer).


## **[Skiplist](skiplist/)**

An attmept to make the multithreaded Skiplist. **Needs work.**
