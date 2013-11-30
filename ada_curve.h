// ada_curve.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei.ren@stanzax.org>
// Nov. 29, 2013

#ifndef SESTET_TRACE_ANALYSER_ADA_CURVE_H_
#define SESTET_TRACE_ANALYSER_ADA_CURVE_H_

#include <vector>
#include "o_curve.h"

// Ported from GSL for simulation
double
gsl_fit_linear (const std::vector<double> &x, const std::vector<double> &y,
                const int n);

class AdaCurve : public OCurve {
  public:
    AdaCurve(const int history_len, const double threshold,
        SimuEngine &engine) :
        len_(history_len), x_(history_len), y_(history_len), index_(0),
        threshold_(threshold), engine_(engine) { }
    const std::list<OPoint> &tran_points() const { return tran_points_; }

    void OnWrite(const DataItem &item, bool hit) {
      OCurve::OnWrite(item ,hit);

      tran_stale_ += PAGE_SIZE;
      if (hit) tran_overwritten_ += PAGE_SIZE;
      x_[index_] = (double)tran_stale_ / 1024;
      y_[index_] = (double)tran_overwritten_ / tran_stale_;

      OPoint p = { item.di_time, staleness() / 1024, y_[index_] };
      tran_points_.push_back(p);

      if (gsl_fit_linear(x_, y_, len_) < threshold_) Clear();
      inc_index();
    }

    void OnEvict(const DataItem &item, bool hit) {
      OCurve::OnEvict(item, hit);
      if (hit) {
        tran_overwritten_ += PAGE_SIZE;
        // Instant modification may help to smooth curve.
        y_[last_index()] = (double)tran_overwritten_ / tran_stale_;
        tran_points_.back().p_oratio = y_[last_index()];
      }
    }

  private:
    unsigned int last_index() const {
      return index_ == 0 ? (len_ - 1) : (index_ - 1);
    }
    void inc_index() { index_ == len_ ? index_ = 0 : ++index_; }
    void Clear() {
      tran_stale_ = 0;
      tran_overwritten_ = 0;
      engine_.Clear();
    }

    unsigned int index_;
    const unsigned int len_;
    std::vector<double> x_;
    std::vector<double> y_;

    unsigned long tran_stale_;
    unsigned long tran_overwritten_;
    const double threshold_;
    SimuEngine &engine_;
    std::list<OPoint> tran_points_;
};

// Ported from GSL for simulation
double
gsl_fit_linear (const std::vector<double> &x, const std::vector<double> &y,
                const int n)
{
  double m_x = 0, m_y = 0, m_dx2 = 0, m_dxdy = 0;

  int i;

  for (i = 0; i < n; i++)
    {
      m_x += (x[i] - m_x) / (i + 1.0);
      m_y += (y[i] - m_y) / (i + 1.0);
    }

  for (i = 0; i < n; i++)
    {
      const double dx = x[i] - m_x;
      const double dy = y[i] - m_y;

      m_dx2 += (dx * dx - m_dx2) / (i + 1.0);
      m_dxdy += (dx * dy - m_dxdy) / (i + 1.0);
    }

  /* In terms of y = a + b x */

  return m_dxdy / m_dx2;
}

#endif // SESTET_TRACE_ANALYSER_ADA_CURVE_H_
