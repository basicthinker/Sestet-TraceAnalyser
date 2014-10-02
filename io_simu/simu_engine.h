// simu_engine.h
// Sestet-TraceAnayliser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 25, 2013

#ifndef SESTET_TRACE_ANALYSER_SIMU_ENGINE_H_
#define SESTET_TRACE_ANALYSER_SIMU_ENGINE_H_

#include <iostream>
#include <set>
#include <list>

#include "data_item.h"
#include "simu_state.h"

class SimuEngine {
  public:
    void Register(SimuState &state);
    void Input(DataOperation op, const struct DataItem &item);
    void Clear();
    bool is_fsync_flush() { return fsync_flush_; }
    void set_fsync_flush(bool is_flush) { fsync_flush_ = is_flush; } 
 private:
    bool fsync_flush_;
    std::set<DataTag> cache_; // simulates page cache
    std::list<SimuState *> states_;
};

// Implementations
using std::cerr;
using std::endl;

inline void SimuEngine::Register(SimuState &state) {
  states_.push_back(&state);
}

void SimuEngine::Input(DataOperation op, const struct DataItem &item) {
  DataTag tag(item.di_file, item.di_index);
  bool hit = (cache_.find(tag) != cache_.end());
  std::list<SimuState *>::iterator state_i;

  switch (op) {
  case kDataRead:
    for (state_i = states_.begin(); state_i != states_.end(); ++state_i) {
      (*state_i)->OnRead(item, hit);
    }
    break;
  case kDataWrite:
    if (!hit) cache_.insert(tag);
    for (state_i = states_.begin(); state_i != states_.end(); ++state_i) {
      (*state_i)->OnWrite(item, hit);
    }
    break;
  case kDataEvict:
    if (hit) cache_.erase(tag);
    for (state_i = states_.begin(); state_i != states_.end(); ++state_i) {
      (*state_i)->OnEvict(item, hit);
    }
    break;
  case kDataFsync:
    if (item.di_index != 0) { // integrity check
      cerr << "Warning: integrity check fails:"
          " fsync item has non-zero index." << endl;
      break;
    }
    for (state_i = states_.begin(); state_i != states_.end(); ++state_i) {
      (*state_i)->OnFsync(item);
    }

    if (fsync_flush_) {
      for (std::set<DataTag>::iterator tag_i = cache_.begin();
          tag_i != cache_.end(); ) {
        if (tag_i->first == item.di_file) {
          DataItem flushed = { item.di_time, tag_i->first, tag_i->second };
          cache_.erase(tag_i++);
          for (state_i = states_.begin(); state_i != states_.end(); ++state_i) {
            (*state_i)->OnFlush(flushed);
          }
        } else ++tag_i;
      }
    }
    break;
 defalt:
    cerr << "Warning: invalid trace entry type: " << op << endl;
  }
}

void SimuEngine::Clear() {
  for (std::set<DataTag>::iterator i = cache_.begin();
      i != cache_.end(); ++i) {
    DataItem flushed = { -1, i->first, i->second };
    for (std::list<SimuState *>::iterator j = states_.begin();
        j != states_.end(); ++j) {
      (*j)->OnFlush(flushed);
    }
  }
  cache_.clear();
}

#endif // SESTET_TRACE_ANALYSER_SIMU_ENGINE_H_
