#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: $0 [TRACE DIR]"
  exit 1
fi

trace_dir=$1
for trace_file in `ls $trace_dir/*-io-*.trace`
do
  trace_txt=$trace_file'.txt'
  fsyncs_txt=${trace_file%'.trace'}'.fsyncs'
  eps_file=${trace_file%'.trace'}'.eps'
  ./curves.out $trace_file $trace_txt
  ./fsyncs.out $trace_file $fsyncs_txt
  gnuplot -e "OUTPUT_PLT='$eps_file'" -e "TRACE_TXT='$trace_txt'" -e "FSYNCS_TXT='$fsyncs_txt'" opt-ratio.plt
  if [ $? != 0 ]; then
    echo "Error: failed to draw $trace_file"
  fi
done

