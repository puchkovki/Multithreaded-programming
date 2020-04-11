#!/bin/bash
echo "Plotting metrics"
export DISPLAY=:0.0
gnuplot <<< "set terminal png size 1024, 720; \
             set xlabel 'Number of processes'; \
             set ylabel 'Matrix rank'; \
             set zlabel 'Time'; \
             set output 'res/3d.png'; \
             splot 'res/3d.txt' with linespoints; "
echo "Plot image stored at res"
exit $?
#pm3d