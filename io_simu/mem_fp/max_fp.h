// max_fp.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Sept. 29, 2014

#ifndef SESTET_TRACE_ANALYSER_MAX_FP_H_
#define SESTET_TRACE_ANALYSER_MAX_FP_H_

#include <set>
#include "../simu_state.h"

class MaxFootprint : public SimuState {
 public:
  MaxFootprint() : duration_(0) { }
  void OnRead(const DataItem &item, bool hit);
  void OnWrite(const DataItem &item, bool hit);
  void Clear() { cache_.clear(); }
  unsigned long GetSize() const { return cache_.size(); }
  double duration() const { return duration_; }

 protected:
  std::set<DataTag> cache_;
  double duration_;
 
 private:
  void Access(double time, unsigned long file, unsigned long index);
};

void MaxFootprint::OnRead(const DataItem &item, bool hit) {
  Access(item.di_time, item.di_file, item.di_index);
}

void MaxFootprint::OnWrite(const DataItem &item, bool hit) {
  Access(item.di_time, item.di_file, item.di_index);
}

void MaxFootprint::Access(double time,
    unsigned long file, unsigned long index) {
  DataTag tag(file, index);
  cache_.insert(tag);
  duration_ = time;
}

#endif // SESTET_TRACE_ANALYSER_MAX_FP_H_

