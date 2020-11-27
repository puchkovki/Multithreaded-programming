## **Задача**

Напишите программу c использованием `pthreads` или `C++11 threads`, где 4 потока 
заполняют промежуточный буфер (единый для всех потоков) символами `'1'`, `'2'`, 
`'3'` или `'4'` соответственно, причём после 100 символов поток заканчивает 
свою работу, и ещё 4 потока читают данные из буфера и пишут в 4 файла, причём 
каждый поток должен считать из буфера 100 символов.

## **Task**

Write program producing 4 threads, which fill shared buffer with 100 characters 
`'1'`, `'2'`, `'3'` and `'4'` correspondingly, using `pthreads` or 
`C+11 threads` library. Threads stop after writing 100 characters, and other 4 
threads read 100 characters from the buffer and put them into 4 different data 
files.