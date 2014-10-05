// passive_curve.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Oct. 5, 2014

#ifndef SESTET_TRACE_ANALYSER_PASSIVE_CURVE_H_
#define SESTET_TRACE_ANALYSER_PASSIVE_CURVE_H_

#include <list>
#include "data_item.h"
#include "simu_state.h"
#include "o_curve.h"
#include "cache.h"

class PassiveCurve : public SimuState {
 public:
  const std::list<OPoint> &GetPoints() const { return curve_.points(); }

  virtual void OnWrite(const DataItem &item);
  virtual void OnEvict(const DataItem &item);
  virtual void OnFsync(double time, unsigned long file);
  virtual void OnFlush(double time);

 private:
  Cache write_cache_;
  OCurve curve_;
};

void PassiveCurve::OnWrite(const DataItem &item) {
  bool hit = write_cache_.Insert(item.di_file, item.di_index);
  curve_.BlockWrite(item.di_time, hit);
}

void PassiveCurve::OnEvict(const DataItem &item) {
  bool hit = write_cache_.Erase(item.di_file, item.di_index);
  if (hit) {
    curve_.BlockEvict();
  }
}

void PassiveCurve::OnFsync(double time, unsigned long file) {
  write_cache_.Erase(file);
}

void PassiveCurve::OnFlush(double time) {
  write_cache_.Clear();
}

#endif // SESTET_TRACE_ANALYSER_PASSIVE_CURVE_H_

