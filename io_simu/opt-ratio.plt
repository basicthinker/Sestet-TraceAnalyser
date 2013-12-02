set terminal postscript eps color
set output OUTPUT_PLT
set ylabel '% Overwritten'
set yrange [0:100]
set xlabel 'Total Data Written (MB)'

plot TRACE_TXT using 2:4 title 'Ideal' with lines, \
  ADA_TXT using 2:3 title 'AdaFS' with lines, \
  TRACE_TXT using 2:3 title 'Ext4' with lines, \
  ADA_TXT using 2:4 title 'AdaFS VCTs' with linespoints pt 6 ps 0.5, \
  FSYNCS_TXT using 2:3 title 'fsync' with points pt 1 
