#!/bin/bash
echo "Setting environment"
DIRNAME=`dirname "$0"`
CPPFLAGS="-pthread -Wall -Wextra -pedantic -std=c++17 -O3 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address,undefined -fno-sanitize-recover=all -fstack-protector"
rm -rf ${DIRNAME}/res/
mkdir ${DIRNAME}/res
mkdir ${DIRNAME}/res/num_i
mkdir ${DIRNAME}/res/size_i

echo "Compiling"
g++ ${CPPFLAGS} -o ${DIRNAME}/a.out ${DIRNAME}/main.cpp

echo "Started"

echo "num_i_size_100_many_1"
echo -n > ${DIRNAME}/res/num_i/num_i_size_100_many_1.txt
for ((i = 1; i < 50; ++i))
do
${DIRNAME}/a.out $i 100 1 >> ${DIRNAME}/res/num_i/num_i_size_100_many_1.txt
done

echo "num_i_size_1000_many_1"
echo -n > ${DIRNAME}/res/num_i/num_i_size_1000_many_1.txt
for ((i = 1; i < 50; ++i))
do
${DIRNAME}/a.out $i 1000 1 >> ${DIRNAME}/res/num_i/num_i_size_1000_many_1.txt
done

echo "num_1_size_i_many_1"
echo -n > ${DIRNAME}/res/size_i/num_1_size_i_many_1.txt
for ((i = 1; i < 500; ++i))
do
${DIRNAME}/a.out 1 $i 1 >> ${DIRNAME}/res/size_i/num_1_size_i_many_1.txt
done

echo "num_2_size_i_many_1"
echo -n > ${DIRNAME}/res/size_i/num_2_size_i_many_1.txt
for ((i = 1; i < 500; ++i))
do
${DIRNAME}/a.out 2 $i 1 >> ${DIRNAME}/res/size_i/num_2_size_i_many_1.txt
done

echo "num_4_size_i_many_1"
echo -n > ${DIRNAME}/res/size_i/num_4_size_i_many_1.txt
for ((i = 1; i < 500; ++i))
do
${DIRNAME}/a.out 4 $i 1 >> ${DIRNAME}/res/size_i/num_4_size_i_many_1.txt
done

echo "num_8_size_i_many_1"
echo -n > ${DIRNAME}/res/size_i/num_8_size_i_many_1.txt
for ((i = 1; i < 500; ++i))
do
${DIRNAME}/a.out 8 $i 1 >> ${DIRNAME}/res/size_i/num_8_size_i_many_1.txt
done

echo "num_16_size_i_many_1"
echo -n > ${DIRNAME}/res/size_i/num_16_size_i_many_1.txt
for ((i = 1; i < 500; ++i))
do
${DIRNAME}/a.out 16 $i 1 >> ${DIRNAME}/res/size_i/num_16_size_i_many_1.txt
done

echo "Ended"

echo "Plotting metrics"
gnuplot <<< "set terminal png size 1024, 720; \
            set xlabel 'Number of processes'; \
            set ylabel 'Time'; \
            set output 'res/num_i/Rank_100&1000.png'; \
            plot 'res/num_i/num_i_size_100_many_1.txt' u 1:3 title '100*100' w linespoints, 'res/num_i/num_i_size_1000_many_1.txt' u 1:3 title '1000*1000' w linespoints; \
            set terminal png size 1024, 720; \
            set xlabel 'Matrix rank'; \
            set ylabel 'Time'; \
            set output 'res/size_i/Number_of_processes_1&2&4&8&16.png'; \
            plot 'res/size_i/num_1_size_i_many_1.txt' u 2:3 title '1 process' w linespoints, 'res/size_i/num_2_size_i_many_1.txt' u 2:3 title '2 processes' w linespoints, 'res/size_i/num_4_size_i_many_1.txt' u 2:3 title '4 processes' w linespoints, 'res/size_i/num_8_size_i_many_1.txt' u 2:3 title '8 processes' w linespoints, 'res/size_i/num_16_size_i_many_1.txt' u 2:3 title '16 processes' w linespoints; "
echo "Plot image stored at res"
exit $?
