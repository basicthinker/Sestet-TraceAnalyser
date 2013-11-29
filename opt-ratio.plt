set terminal postscript eps color
set output OUTPUT_PLT
set ylabel '% Overwritten'
set yrange [0:100]
set xlabel 'Total Data Written (MB)'

plot TRACE_TXT using 2:3 title 'Ext4' with lines, \
  '' using 2:4 title 'Ideal' with lines, \
  FSYNCS_TXT using 2:3 title 'fsync' with points pt 1 
