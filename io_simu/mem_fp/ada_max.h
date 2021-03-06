// ada_max.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Oct. 1, 2014

#ifndef SESTET_TRACE_ANALYSER_ADA_MAX_H_
#define SESTET_TRACE_ANALYSER_ADA_MAX_H_

#include <vector>
#include "../simu_state.h"
#include "versioned_cache.h"
#include "integral_avg.h"

// Ported from GSL for simulation
double
gsl_fit_linear (const std::vector<double> &x, const std::vector<double> &y,
                const int n);

class AdaMaxFP : public SimuState {
 public:
  AdaMaxFP(int history_len, double threshold, int min_stale, int max_stale) :
      len_(history_len), x_(history_len), y_(history_len), index_(0),
      tran_stale_(0), tran_overwritten_(0),
      min_stale_(min_stale), max_stale_(max_stale), threshold_(threshold) {
  }

  int num_trans() const { return rw_cache_.version(); }
  double GetAverage() const { return average_.GetAverage(); }

  void OnRead(const DataItem &item);
  void OnWrite(const DataItem &item);
  void OnEvict(const DataItem &item);

 private:
  void NewTransaction(double time);
  unsigned int last_index() const {
    return index_ == 0 ? (len_ - 1) : (index_ - 1);
  }
  void inc_index() { index_ == len_ ? index_ = 0 : ++index_; }

  unsigned int index_;
  const unsigned int len_;
  std::vector<double> x_;
  std::vector<double> y_;

  unsigned long tran_stale_;
  unsigned long tran_overwritten_;
  const int min_stale_;
  const int max_stale_;
  const double threshold_;
  VersionedCache rw_cache_;
  IntegralAverage average_;
};

void AdaMaxFP::NewTransaction(double time) {
  if (tran_stale_ < min_stale_) return;
  tran_stale_ = 0;
  tran_overwritten_ = 0;
  rw_cache_.IncVersion();
}

void AdaMaxFP::OnRead(const DataItem &item) {
  rw_cache_.Insert(item.di_file, item.di_index);
  average_.Input(item.di_time, rw_cache_.Size());
}

void AdaMaxFP::OnWrite(const DataItem &item) {
  bool hit = rw_cache_.Insert(item.di_file, item.di_index);
  average_.Input(item.di_time, rw_cache_.Size());

  tran_stale_ += 1;
  if (hit) tran_overwritten_ += 1;
  x_[index_] = (double)tran_stale_;
  y_[index_] = (double)tran_overwritten_ / tran_stale_ * 100;

  if (tran_stale_ >= max_stale_ ||
      gsl_fit_linear(x_, y_, len_) < threshold_) {
    NewTransaction(item.di_time);
  }
  inc_index();
}

void AdaMaxFP::OnEvict(const DataItem &item) {
  bool hit = rw_cache_.Erase(item.di_file, item.di_index);
  average_.Input(item.di_time, rw_cache_.Size());
  if (hit) {
    tran_overwritten_ += 1;
    // Instant modification may help to smooth curve.
    y_[last_index()] = (double)tran_overwritten_ / tran_stale_ * 100;
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

#endif // SESTET_TRACE_ANALYSER_ADA_MAX_H_

