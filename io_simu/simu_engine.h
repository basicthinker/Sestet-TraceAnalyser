// simu_engine.h
// Sestet-TraceAnayliser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 25, 2013

#ifndef SESTET_TRACE_ANALYSER_SIMU_ENGINE_H_
#define SESTET_TRACE_ANALYSER_SIMU_ENGINE_H_

#include <iostream>
#include <list>

#include "data_item.h"
#include "simu_state.h"

class SimuEngine {
 public:
  void Register(SimuState *state) { states_.push_back(state); }
  virtual void Input(DataOperation op, const struct DataItem &item) = 0;
 protected:
  std::list<SimuState *> states_;
};

class LazyEngine : public SimuEngine {
 public:
  void Input(DataOperation op, const struct DataItem &item);
};

class VFSEngine : public SimuEngine {
 public:
  VFSEngine(double interval) : interval_(interval), next_flush_(interval) { }
  void Input(DataOperation op, const struct DataItem &item);
  double interval() const { return interval_; }
 private:
  const double interval_;
  double next_flush_;
};

// Implementations

void LazyEngine::Input(DataOperation op, const struct DataItem &item) {
  std::list<SimuState *>::iterator si;

  switch (op) {
  case kDataRead:
    for (si = states_.begin(); si != states_.end(); ++si) {
      (*si)->OnRead(item);
    }
    break;
  case kDataWrite:
    for (si = states_.begin(); si != states_.end(); ++si) {
      (*si)->OnWrite(item);
    }
    break;
  case kDataEvict:
    for (si = states_.begin(); si != states_.end(); ++si) {
      (*si)->OnEvict(item);
    }
    break;
  case kDataFsync:
    break;
  defalt:
    std::cerr << "Warning: invalid trace entry type: " << op << std::endl;
  }
}

void VFSEngine::Input(DataOperation op, const struct DataItem &item) {
  std::list<SimuState *>::iterator si;

  if (item.di_time > next_flush_) {
    for (si = states_.begin(); si != states_.end(); ++si) {
      (*si)->OnFlush(item.di_time);
    }  
    next_flush_ += interval_;
  }

  switch (op) {
  case kDataRead:
    for (si = states_.begin(); si != states_.end(); ++si) {
      (*si)->OnRead(item);
    }
    break;
  case kDataWrite:
    for (si = states_.begin(); si != states_.end(); ++si) {
      (*si)->OnWrite(item);
    }
    break;
  case kDataEvict:
    for (si = states_.begin(); si != states_.end(); ++si) {
      (*si)->OnEvict(item);
    }
    break;
  case kDataFsync:
    if (item.di_index != 0) { // integrity check
      std::cerr << "Warning: fsync item has non-zero index." << std::endl;
      break;
    }
    for (si = states_.begin(); si != states_.end(); ++si) {
      (*si)->OnFsync(item.di_time, item.di_file);
    }
    break;
  defalt:
    std::cerr << "Warning: invalid trace entry type: " << op << std::endl;
  }
}

#endif // SESTET_TRACE_ANALYSER_SIMU_ENGINE_H_

