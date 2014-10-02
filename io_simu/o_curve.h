// o_curve.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 28, 2013

#ifndef SESTET_TRACE_ANALYSER_O_CURVE_H_
#define SESTET_TRACE_ANALYSER_O_CURVE_H_

#include "data_item.h"
#include "simu_state.h"
#include "simu_engine.h"
#include <list>

class OPoint {
  public:
    OPoint(double time, unsigned long blocks, double ratio) {
      set_time(time);
      set_staleness(blocks);
      set_ratio(ratio);
    }
    void set_time(double time) { time_ = time; }
    double time() const { return time_; }

    void set_staleness(unsigned long blocks) { stale_blocks_ = blocks; }
    unsigned long stale_blocks() const { return stale_blocks_; }
    
    void set_ratio(double ratio) { ratio_ = ratio; }
    double percent() const { return ratio_ * 100; }

  private:
    double time_;
    unsigned long stale_blocks_;
    double ratio_; // 0~1
};

class OCurve : public SimuState {
  public:
    OCurve() : stale_(0), overwritten_(0) { }
    const std::list<OPoint> &points() const { return points_; }
    unsigned long staleness() const { return stale_; }

    virtual void OnWrite(const DataItem &item, bool hit) {
      stale_ += 1;
      if (hit) overwritten_ += 1;

      OPoint p(item.di_time, stale_, (double)overwritten_ / stale_);
      points_.push_back(p);
    }

    virtual void OnEvict(const DataItem &item, bool hit) {
      if (hit) {
        overwritten_ += 1;
        points_.back().set_ratio((double)overwritten_ / stale_);
      }
    }

  private:
    unsigned long stale_;
    unsigned long overwritten_;
    std::list<OPoint> points_;
};

#endif // SESTET_TRACE_ANALYSER_O_CURVE_H_
