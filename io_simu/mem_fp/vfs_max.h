// vfs_max.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Sept. 29, 2014

#ifndef SESTET_TRACE_ANALYSER_VFS_MAX_FP_H_
#define SESTET_TRACE_ANALYSER_VFS_MAX_FP_H_

#include <set>
#include "../cache.h"
#include "../simu_state.h"
#include "integral_avg.h"

class VFSMaxFP : public SimuState {
 public:
  VFSMaxFP() : duration_(0) { }
  unsigned long GetSize() const { return rw_cache_.Size(); }
  double GetAverage() const { return average_.GetAverage(); }
  double duration() const { return duration_; }

  void OnRead(const DataItem &item);
  void OnWrite(const DataItem &item);
  void OnEvict(const DataItem &item);

 private:
  void Access(double time, unsigned long file, unsigned long index);

  Cache rw_cache_;
  IntegralAverage average_;
  double duration_;
};

void VFSMaxFP::OnRead(const DataItem &item) {
  Access(item.di_time, item.di_file, item.di_index);
}

void VFSMaxFP::OnWrite(const DataItem &item) {
  Access(item.di_time, item.di_file, item.di_index);
}

void VFSMaxFP::Access(double time,
    unsigned long file, unsigned long index) {
  rw_cache_.Insert(file, index);
  average_.Input(time, rw_cache_.Size());
  duration_ = time;
}

void VFSMaxFP::OnEvict(const DataItem &item) {
  rw_cache_.Erase(item.di_file, item.di_index);
  average_.Input(item.di_time, rw_cache_.Size());
  duration_ = item.di_time;
}

#endif // SESTET_TRACE_ANALYSER_VFS_MAX_FP_H_

