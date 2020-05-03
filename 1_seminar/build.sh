#!/bin/bash
FULLDIR=$(dirname "$0")
g++ -Wall -Wextra -pedantic -O3 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address,undefined -fno-sanitize-recover=all -fstack-protector -std=c++17 -pthread -o $FULLDIR/test $FULLDIR/thread_c++.cpp || exit 1
$FULLDIR/test $1
exit $?