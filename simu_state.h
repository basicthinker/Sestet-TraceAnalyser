// simu_state.h
// Sestet-TraceAnayliser
//
// Jinglei Ren <jinglei.ren@stanzax.org>
// Nov. 26, 2013

#ifndef SESTET_TRACE_ANALYSER_SIMU_STATE_H_
#define SESTET_TRACE_ANALYSER_SIMU_STATE_H_

#include "data_item.h"

class SimuState {
  public:
    virtual void OnRead(const DataItem &item, bool hit) { }
    virtual void OnWrite(const DataItem &item, bool hit) { }
    virtual void OnEvict(const DataItem &item, bool hit) { }
    virtual void OnFlush(const DataItem &item) { }
    virtual void OnFsync(const DataItem &item) { }
};

#endif // SESTET_TRACE_ANALYSER_SIMU_STATE_H_

