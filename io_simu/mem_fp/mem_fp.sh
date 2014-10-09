#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: $0 [TRACE DIR] [MIN STALE=128] [MAX STALE=25600]" \
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
  max_stale=$3
else
  max_stale='25600'
fi

if [ $# -ge 4 ]; then
  ada_threshold=$4
else
  ada_threshold='-0.02'
fi

if [ $# -ge 5 ]; then
  ada_len=$5
else
  ada_len='16'
fi

for trace_file in `ls $trace_dir/*-io-*.trace`
do
  ./mem_fp.out $trace_file $min_stale $max_stale $ada_threshold $ada_len
done

