#!/bin/bash
echo "Plotting metrics"
gnuplot <<< "set term png size 1920,1080; \
             set output 'seminar_3/build/ttas.png'; \
             set multiplot layout 2, 1 title 'ttas'; \
             set tmargin 2; \
             set notitle; \
	         plot 'seminar_3/build/puchkovki_ttas.dat' u 1:2 title 'overall time(ms) puchkovki' w linespoints, 'seminar_3/build/trmigor_ttas.dat' u 1:2 title 'trmigor' w linespoints;\
             plot 'seminar_3/build/puchkovki_ttas.dat' u 1:3 title 'average latency(cpu ticks) per iteration puchkovki' w linespoints, 'seminar_3/build/trmigor_ttas.dat' u 1:3 title 'trmigor' w linespoints; \
             unset multiplot; \
             set term png size 1920,1080; \
             set output 'seminar_3/build/ticket.png'; \
             set multiplot layout 2, 1 title 'ticket'; \
             set tmargin 2; \
             set notitle; \
	         plot 'seminar_3/build/puchkovki_ticket.dat' u 1:2 title 'overall time(ms) puchkovki' w linespoints, 'seminar_3/build/trmigor_ticket.dat' u 1:2 title 'trmigor' w linespoints;\
             plot 'seminar_3/build/puchkovki_ticket.dat' u 1:3 title 'average latency(cpu ticks) per iteration puchkovki' w linespoints, 'seminar_3/build/trmigor_ticket.dat' u 1:3 title 'trmigor' w linespoints;"
echo "Plotting image done"