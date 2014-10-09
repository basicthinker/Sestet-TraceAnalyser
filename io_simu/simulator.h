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
  Simulator &Register(SimuEngine &engine);
  double Run();
 protected:
  void FeedEngines(DataOperation op, const struct DataItem &item);
  TraceParser parser_;
  std::list<SimuEngine *> engines_;
};

Simulator &Simulator::Register(SimuEngine &engine) {
  engines_.push_back(&engine);
  return *this;
}

void Simulator::FeedEngines(DataOperation op, const struct DataItem &item) {
  for (std::list<SimuEngine *>::iterator it = engines_.begin();
      it != engines_.end(); ++it) {
    (*it)->Input(op, item);
  }
}

double Simulator::Run() {
  DataOperation op;
  DataItem item;

  if (!parser_.Next(op, item)) return 0;
  const double begin_time = item.di_time;

  item.di_time -= begin_time;
  double duration = item.di_time;
  FeedEngines(op, item);
  while (parser_.Next(op, item)) {
    item.di_time -= begin_time;
    duration = item.di_time;
    FeedEngines(op, item);
  }
  return duration;
}

#endif // SESTET_TRACE_ANALYSER_SIMULATOR_H_

