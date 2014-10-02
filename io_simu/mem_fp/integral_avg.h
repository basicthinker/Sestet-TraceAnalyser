// integral_avg.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Sept. 29, 2014

#ifndef SESTET_TRACE_ANALYSER_INTEGRAL_AVG_H_
#define SESTET_TRACE_ANALYSER_INTEGRAL_AVG_H_

#include <cassert>
#include <limits>

#define DBL_MIN (std::numeric_limits<double>::lowest())

class IntegralAverage {
 public:
  IntegralAverage();
  void Input(double x, double y);
  double GetAverage() const;

  double integral() const { return integral_; }
 private:
  double init_x_;
  double last_x_;
  double last_y_;
  double integral_;
};

IntegralAverage::IntegralAverage() :
    init_x_(DBL_MIN), last_x_(DBL_MIN),
    last_y_(DBL_MIN) {
  integral_ = 0;
}

void IntegralAverage::Input(double x, double y) {
  if (init_x_ == DBL_MIN) {
    assert(last_x_ == DBL_MIN && last_y_ == DBL_MIN);
    init_x_ = x;
    last_x_ = x;
    last_y_ = y;
  } else {
    assert(x >= last_x_);
    integral_ += (x - last_x_) * last_y_;

    last_x_ = x;
    last_y_ = y;
  }
}

double IntegralAverage::GetAverage() const {
  if (last_x_ == init_x_) return 0;
  else return integral_ / (last_x_ - init_x_);
}

#endif // SESTET_TRACE_ANALYSER_INTEGRAL_AVG_H_

