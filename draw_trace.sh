#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: $0 [TRACE DIR] [ADA LEN = 16] [ADA THRESHOLD = -0.1]"
  exit 1
fi

trace_dir=$1
if [ $# -ge 2 ]; then
  ada_len=$2
else
  ada_len=16
fi
if [ $# -eq 3 ]; then
  ada_threshold=$3
else
  ada_threshold='-0.1'
fi

for trace_file in `ls $trace_dir/*-io-*.trace`
do
  trace_txt=$trace_file'.txt'
  ada_txt=${trace_file%'.trace'}'.ada.txt'
  fsyncs_txt=${trace_file%'.trace'}'.fsyncs'
  eps_file=${trace_file%'.trace'}'.eps'

  if [ ! -f $trace_txt ]; then
    ./curves.out $trace_file $trace_txt
  fi
  if [ ! -f $fsyncs_txt ]; then
    ./fsyncs.out $trace_file $fsyncs_txt
  fi
  ./ada_curve.out $trace_file $ada_txt $ada_len $ada_threshold

  gnuplot -e "OUTPUT_PLT='$eps_file'" -e "TRACE_TXT='$trace_txt'" -e "ADA_TXT='$ada_txt'" -e "FSYNCS_TXT='$fsyncs_txt'" opt-ratio.plt
  if [ $? != 0 ]; then
    echo "Error: failed to draw $trace_file"
  fi
done

