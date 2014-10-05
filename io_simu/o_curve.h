// o_curve.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 28, 2013

#ifndef SESTET_TRACE_ANALYSER_O_CURVE_H_
#define SESTET_TRACE_ANALYSER_O_CURVE_H_

#include <list>

class OPoint {
  public:
    OPoint(double time, unsigned long blocks, double ratio) :
        time_(time), stale_blocks_(blocks), ratio_(ratio) {
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

class OCurve {
 public:
  OCurve() : stale_(0), overwritten_(0), global_stale_(0) { }
  const std::list<OPoint> &points() const { return points_; }
  unsigned long stale() const { return stale_; }
  double GetRatio() const { return (double)overwritten_ / stale_; }
  unsigned long global_stale() const { return global_stale_; }

  void BlockWrite(double time, bool hit);
  void BlockEvict();
  void NewTransaction();

 private:
  unsigned long stale_;
  unsigned long overwritten_;
  std::list<OPoint> points_;
  unsigned long global_stale_;
};

void OCurve::BlockWrite(double time, bool hit) {
  stale_ += 1;
  if (hit) overwritten_ += 1;
  global_stale_ += 1;
  OPoint p(time, global_stale(), GetRatio());
  points_.push_back(p);
}

void OCurve::BlockEvict() {
  overwritten_ += 1;
  points_.back().set_ratio(GetRatio());
}

void OCurve::NewTransaction() {
  stale_ = 0;
  overwritten_ = 0;
}

#endif // SESTET_TRACE_ANALYSER_O_CURVE_H_

