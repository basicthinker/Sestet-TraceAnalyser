#!/bin/bash

if [ $# -ne 3 ]; then
  echo "Usage: $0 [TRACE DIR] [STAT FILE] [MAX MULTIPLE]"
  exit 1
fi

trace_dir=$1
stat_file=$2
max_multi=$3

if [ -f $stat_file ]; then
  mv $stat_file $stat_file.old
fi

for ev_file in `ls $trace_dir/*-ev-*.trace`
do
  int_file=${ev_file%'.trace'}'.int'

  if [ ! -f $int_file ]; then
    python filter-ev-trace.py $ev_file > $int_file
  fi
  for ((thre_multi=1; thre_multi<=$max_multi; ++thre_multi));
  do
    ./fsm_avg_int_pred.out $int_file $thre_multi >> $stat_file 2>/dev/null
    ./fsm_min_int_pred.out $int_file $thre_multi >> $stat_file 2>/dev/null
    ./fsm_mix_int_pred.out $int_file $thre_multi >> $stat_file 2>/dev/null
    ./last_int_pred.out $int_file >> $stat_file 2>/dev/null
  done
done

