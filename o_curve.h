// o_curve.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei.ren@stanzax.org>
// Nov. 28, 2013

#ifndef SESTET_TRACE_ANALYSER_O_CURVE_H_
#define SESTET_TRACE_ANALYSER_O_CURVE_H_

#include "data_item.h"
#include "simu_state.h"

#include <list>

// Kernel config
#define PAGE_SIZE 4096

struct OPoint {
  double p_time;
  unsigned long p_stale;
  double p_oratio;
};

class OCurve : public SimuState {
  public:
    OCurve() : stale_(0), overwritten_(0) { }
    const std::list<OPoint> &points() const { return points_; }

    void OnWrite(const DataItem &item, bool hit) {
      stale_ += PAGE_SIZE;
      if (hit) overwritten_ += PAGE_SIZE;

      OPoint p = { item.di_time, stale_, (double)overwritten_ / stale_ };
      points_.push_back(p);
    }

    void OnEvict(const DataItem &item, bool hit) {
      if (hit) {
        overwritten_ += PAGE_SIZE;
        points_.back().p_oratio = (double)overwritten_ / stale_;
      }
    }

  private:
    unsigned long stale_;
    unsigned long overwritten_;
    std::list<OPoint> points_;
};

#endif // SESTET_TRACE_ANALYSER_O_CURVE_H_
