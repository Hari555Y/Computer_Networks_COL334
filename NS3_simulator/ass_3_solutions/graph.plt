set terminal png
set output "Configuration3_Sender3.png"

set title "Congestion Window size(cwnd) vs Time curve"
set xlabel "Time"
set ylabel "Congestion Window Size(cwnd)"
plot "Configuration3connection3.cwnd" using 1:3 with linespoints title "Congestion Window Size"
