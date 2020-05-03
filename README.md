# **Parallel programming DATI**
Parallel programming class for the DCAM MIPT students in Acronis.

## 1_1

Write program receiving input number `N` as an command line argument and making
`N` threads via `pthreads` or `C+11 threads` library. Each thread must print its
executable number in order and its ID.

## 1_2

Write program producing 4 threads, which fill shared buffer with 100 characters 
`'1'`, `'2'`, `'3'` and `'4'` correspondingly, using `pthreads` or 
`C+11 threads` library. Threads stop after writing 100 characters, and other 4 
threads read 100 characters from the buffer and put them into 4 different data 
files.

## 1_3

Write running program executing `ls` in a child process.

## Locks

Realisations of different `std::atomic` locks:
 - Test and test-and-set lock (TTAS)
   - yield
   - exponential backoff (sleep)
 - Ticket lock
   - yield
   - exponential backoff (sleep)

Realization checks on different benchmarks: Phillip's and Vsevolod's.


## Matrix

Realisation of the multithreading block matrix's multiplication. Checked for the 
cache misses with `vagrind`.