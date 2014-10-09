// ada_curves.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 29, 2013

#ifndef SESTET_TRACE_ANALYSER_ADA_CURVES_H_
#define SESTET_TRACE_ANALYSER_ADA_CURVES_H_

#include <vector>
#include "simu_state.h"
#include "o_curve.h"
#include "cache.h"

// Ported from GSL for simulation
double
gsl_fit_linear (const std::vector<double> &x, const std::vector<double> &y,
                const int n);

class AdaCurves : public SimuState {
  public:
    AdaCurves(int history_len, double threshold, int min_stale) :
        len_(history_len), x_(history_len), y_(history_len), index_(0),
        min_stale_(min_stale), threshold_(threshold), flushed_blocks_(0) {
    }
    const std::list<OPoint> &GetPoints() const { return curve_.points(); }
    const std::list<OPoint> &GetTranPoints() const {
      return tran_curve_.points();
    }
    unsigned long flushed_blocks() const { return flushed_blocks_; }

    void OnWrite(const DataItem &item);
    void OnEvict(const DataItem &item);

  private:
    unsigned int last_index() const {
      return index_ == 0 ? (len_ - 1) : (index_ - 1);
    }
    void inc_index() { index_ == len_ ? index_ = 0 : ++index_; }
    unsigned int index_;

    const unsigned int len_;
    std::vector<double> x_;
    std::vector<double> y_;

    const int min_stale_;
    const double threshold_;
    Cache write_cache_;
    OCurve curve_;
    OCurve tran_curve_;
    unsigned long flushed_blocks_;
};

void AdaCurves::OnWrite(const DataItem &item) {
  bool hit = write_cache_.Insert(item.di_file, item.di_index);
  curve_.BlockWrite(item.di_time, hit);
  tran_curve_.BlockWrite(item.di_time, hit);

  x_[index_] = tran_curve_.stale();
  y_[index_] = tran_curve_.GetRatio() * 100;

  if (gsl_fit_linear(x_, y_, len_) < threshold_ &&
      tran_curve_.stale() >= min_stale_) {
    flushed_blocks_ += write_cache_.Size();
    write_cache_.Clear();
    tran_curve_.NewTransaction();
  }
  inc_index();
}

void AdaCurves::OnEvict(const DataItem &item) {
  bool hit = write_cache_.Erase(item.di_file, item.di_index);
  if (hit) {
    curve_.BlockEvict();
    tran_curve_.BlockEvict();
    // Instant modification may help to smooth curve.
    y_[last_index()] = tran_curve_.GetRatio() * 100;
  }
}

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

#endif // SESTET_TRACE_ANALYSER_ADA_CURVES_H_
