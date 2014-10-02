// mem_max.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Sept. 29, 2014

#ifndef SESTET_TRACE_ANALYSER_MEM_MAX_H_
#define SESTET_TRACE_ANALYSER_MEM_MAX_H_

#include <set>
#include "../simu_state.h"

class MemoryMax : public SimuState {
 public:
  MemoryMax() : duration_(0) { }
  void OnRead(const DataItem &item, bool hit);
  void OnWrite(const DataItem &item, bool hit);
  unsigned long GetSize() const { return cache_.size(); }
  double duration() const { return duration_; }

 private:
  std::set<DataTag> cache_;
  void Access(double time, unsigned long file, unsigned long index);
  double duration_;
};

void MemoryMax::OnRead(const DataItem &item, bool hit) {
  Access(item.di_time, item.di_file, item.di_index);
}

void MemoryMax::OnWrite(const DataItem &item, bool hit) {
  Access(item.di_time, item.di_file, item.di_index);
}

void MemoryMax::Access(double time, unsigned long file, unsigned long index) {
  DataTag tag(file, index);
  cache_.insert(tag);
  duration_ = time;
}

#endif // SESTET_TRACE_ANALYSER_MEM_MAX_H_

