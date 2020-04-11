#!/bin/bash
echo "Environment setting."
DIRNAME=`dirname "$0"`
CPPFLAGS="-pthread -Wall -Wextra -pedantic -std=c++17 -O3 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address,undefined -fno-sanitize-recover=all -fstack-protector"
rm -rf ${DIRNAME}/res/
mkdir ${DIRNAME}/res
mkdir ${DIRNAME}/res/num_i
mkdir ${DIRNAME}/res/size_i

echo "Compiling."
g++ ${CPPFLAGS} -o ${DIRNAME}/a.out ${DIRNAME}/main.cpp

echo "Started."

for ((i = 1; i < 50; ++i))
do

for ((j = 1; j < 500; ++j))
do
${DIRNAME}/a.out $i $j 1 >> ${DIRNAME}/res/3d.txt
done
echo.
done


echo "Ended."
