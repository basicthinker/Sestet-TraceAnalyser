// simulator.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 27, 2013

#ifndef SESTET_TRACE_ANALYSER_SIMULATOR_H_
#define SESTET_TRACE_ANALYSER_SIMULATOR_H_

#include "trace_parser.h"
#include "simu_engine.h"

class Simulator {
  public:
    Simulator(const char *file) { parser_.Open(file); }
    virtual void Run() = 0;
    TraceParser &parser() { return parser_; }
    SimuEngine &engine() { return engine_; }
  protected:
    TraceParser parser_;
    SimuEngine engine_;
};

class Ext4Simulator : public Simulator {
  public:
    Ext4Simulator(const char *file) : Simulator(file), interval_(5) {
      engine_.set_to_fsync(true);
    }
    void Run();
    double interval() const { return interval_; }
    void set_interval(double interval) { interval_ = interval; }
  private:
    double interval_;
};

class AdaSimulator : public Simulator {
  public:
    AdaSimulator(const char *file) : Simulator(file) {
      engine_.set_to_fsync(false);
    }
    void Run();
};

// Implementation

void Ext4Simulator::Run() {
  DataOperation op;
  DataItem item;

  if (!parser_.Next(op, item)) return;
  const double begin_time = item.di_time;
  double tran_time = interval_;

  item.di_time -= begin_time;
  engine_.Input(op, item);
  while (parser_.Next(op, item)) {
    item.di_time -= begin_time;
    if (item.di_time > tran_time) {
      engine_.Clear();
      tran_time += interval_;
    }
    engine_.Input(op, item);
  }
}

void AdaSimulator::Run() {
  DataOperation op;
  DataItem item;

  if (!parser_.Next(op, item)) return;
  const double begin_time = item.di_time;

  item.di_time -= begin_time;
  engine_.Input(op, item);
  while (parser_.Next(op, item)) {
    item.di_time -= begin_time;
    engine_.Input(op, item);
  }
}

#endif // SESTET_TRACE_ANALYSER_SIMULATOR_H_
