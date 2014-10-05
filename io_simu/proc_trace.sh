#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: $0 [TRACE DIR] [MIN STALE=128] " \
    "[ADA THRESHOLD = -0.02] [ADA LEN=16]"
  exit 1
fi

trace_dir=$1
if [ $# -ge 2 ]; then
  min_stale=$2
else
  min_stale='128'
fi

if [ $# -ge 3 ]; then
  ada_threshold=$3
else
  ada_threshold='-0.02'
fi

if [ $# -ge 4 ]; then
  ada_len=$4
else
  ada_len='16'
fi

for trace_file in `ls $trace_dir/*-io-*.trace`
do
  trace_txt=$trace_file'.txt'
  ada_txt=${trace_file%'.trace'}'.ada.txt'
  fsyncs_txt=${trace_file%'.trace'}'.fsyncs'
  eps_file=${trace_file%'.trace'}'.eps'

  ./passive_curves.out $trace_file $trace_txt
#  ./fsyncs.out $trace_file $fsyncs_txt
#  ./ada_curves.out $trace_file $ada_txt $min_stale $ada_threshold $ada_len
done

