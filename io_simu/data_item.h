// data_item.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 26, 2013

#ifndef SESTET_TRACE_ANALYSER_DATA_ITEM_H_
#define SESTET_TRACE_ANALYSER_DATA_ITEM_H_

struct DataItem {
  double di_time;
  unsigned long di_ino;
  unsigned long di_index;
};

enum DataOperation {
  kDataRead = 'r',
  kDataWrite = 'w',
  kDataFsync = 's',
  kDataEvict = 'e',
};

#endif // SESTET_TRACE_ANALYSER_DATA_ITEM_H_


