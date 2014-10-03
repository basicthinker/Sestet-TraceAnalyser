// ext4_fp.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Oct. 2, 2014

#ifndef SESTET_TRACE_ANALYSER_EXT4_FP_H_
#define SESTET_TRACE_ANALYSER_EXT4_FP_H_

#include <set>
#include "max_fp.h"
#include "integral_avg.h"

class Ext4Footprint : public MaxFootprint {
 public:
  Ext4Footprint() : num_intervals_(0) { }
  void OnRead(const DataItem &item, bool hit);
  void OnWrite(const DataItem &item, bool hit);

  void ToFsync(double time, unsigned long file);
  void ToClear(double time);

  double GetAverage() const { return average_.GetAverage(); }
  int num_intervals() const { return num_intervals_; }
 private:
  IntegralAverage average_;
  int num_intervals_;
};

void Ext4Footprint::OnRead(const DataItem &item, bool hit) {
  MaxFootprint::OnRead(item, hit);
  average_.Input(item.di_time, cache_.size());
}

void Ext4Footprint::OnWrite(const DataItem &item, bool hit) {
  MaxFootprint::OnWrite(item, hit);
  average_.Input(item.di_time, cache_.size());
}

void Ext4Footprint::ToFsync(double time, unsigned long file) {
  for (std::set<DataTag>::iterator it = cache_.begin();
      it != cache_.end(); ++it) {
    if (it->first == file) {
      cache_.erase(it);
    }
  }  
  duration_ = time;
  average_.Input(time, cache_.size());
}

void Ext4Footprint::ToClear(double time) {
  MaxFootprint::Clear();
  ++num_intervals_;
  duration_ = time;
  average_.Input(time, cache_.size());
}

#endif // SESTET_TRACE_ANALYSER_EXT4_FP_H_

