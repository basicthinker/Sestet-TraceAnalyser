// simu_engine.h // Sestet-TraceAnayliser
//
// Jinglei Ren <jinglei.ren@stanzax.org>
// Nov. 25, 2013

#ifndef SESTET_TRACE_ANALYSER_SIMU_ENGINE_H_
#define SESTET_TRACE_ANALYSER_SIMU_ENGINE_H_

#include <iostream>
#include <set>
#include <list>

#include "data_item.h"
#include "simu_state.h"

using namespace std;

typedef std::pair<unsigned long, double> PageTag;

class SimuEngine {
  public:
    void Register(SimuState &state);
    void Input(DataOperation op, const struct DataItem &item);
    void Clear() { cache_.clear(); }
    bool to_fsync() { return to_fsync_; }
    void set_to_fsync(bool to_fsync) { to_fsync_ = to_fsync; } 
 private:
    bool to_fsync_;
    std::set<PageTag> cache_; // simulates page cache
    std::list<SimuState *> states_;
};

// Implementations

using namespace std;

inline void SimuEngine::Register(SimuState &state) {
  states_.push_back(&state);
}

void SimuEngine::Input(DataOperation op, const struct DataItem &item) {
  PageTag tag(item.di_ino, item.di_index);
  bool hit = (cache_.find(tag) != cache_.end());
  list<SimuState *>::iterator state_i;

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
    if (to_fsync_) {
      for (set<PageTag>::iterator tag_i = cache_.begin();
          tag_i != cache_.end(); ) {
        if (tag_i->first == item.di_ino) {
          cache_.erase(tag_i++);
          for (state_i = states_.begin(); state_i != states_.end(); ++state_i) {
            (*state_i)->OnFlush(item);
          }
        } else ++tag_i;
      }
    }
    break;
 defalt:
    cerr << "Warning: invalid trace entry type: " << op << endl;
  }
}

#endif // SESTET_TRACE_ANALYSER_SIMU_ENGINE_H_
