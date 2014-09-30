// data_item.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 26, 2013

#ifndef SESTET_TRACE_ANALYSER_DATA_ITEM_H_
#define SESTET_TRACE_ANALYSER_DATA_ITEM_H_

struct DataItem {
  double di_time;
  unsigned long di_file;
  unsigned long di_index;
};

typedef std::pair<unsigned long, unsigned long> DataTag;

enum DataOperation {
  kDataRead = 'r',
  kDataWrite = 'w',
  kDataFsync = 's',
  kDataEvict = 'e',
};

#endif // SESTET_TRACE_ANALYSER_DATA_ITEM_H_
