#!/bin/bash

if [ $# -ne 2 ]; then
  echo "Usage: $0 [TRACE DIR] [MULTIPLE]"
  exit 1
fi

trace_dir=$1
thre_multi=$2

num_conflicts=0
num_pred=0
total_len=0
for ev_file in `ls $trace_dir/*-ev-*.trace`
do
  int_file=${ev_file%'.trace'}'.int'
  stat_file=${ev_file%'.trace'}'.stat'

  python filter-ev-trace.py $ev_file > $int_file
  ./simu_int_pred.out $int_file $thre_multi > $stat_file
  ./fsm_int_pred.out $int_file >> $stat_file

  tmp=(`cat $stat_file`)
  num_conf=`expr $num_conf + ${tmp[1]}`
  num_pred=`expr $num_pred + ${tmp[2]}`
  total_len=`echo $total_len + ${tmp[3]} | bc -l`
done

avg_len=`echo $total_len / $num_pred | bc -l`
echo "No. Conflicts = $num_conf, Total Len. = $total_len, No. Pred. = $num_pred, Avg. Len. = $avg_len"

