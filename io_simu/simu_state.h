// simu_state.h
// Sestet-TraceAnayliser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 26, 2013

#ifndef SESTET_TRACE_ANALYSER_SIMU_STATE_H_
#define SESTET_TRACE_ANALYSER_SIMU_STATE_H_

#include "data_item.h"

class SimuState {
  public:
    virtual void OnRead(const DataItem &item) { }
    virtual void OnWrite(const DataItem &item) { }
    virtual void OnEvict(const DataItem &item) { }

    virtual void OnFsync(double time, unsigned long file) { }
    virtual void OnFlush(double time) { }
};

#endif // SESTET_TRACE_ANALYSER_SIMU_STATE_H_

