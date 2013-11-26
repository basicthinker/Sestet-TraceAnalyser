// simu_core.h
// Sestet-TraceAnayliser
//
// Jinglei Ren <jinglei.ren@stanzax.org>
// Nov. 25, 2013

#ifndef SESTET_TRACE_ANALYSER_CORE_H_
#define SESTET_TRACE_ANALYSER_CORE_H_

#include <stdint.h>
#include <set>

#include "kernel_struct.h"

typedef std::pair<unsigned long, double> PageTag;

class SimuCore {
  public:
    void Input(char operation, const struct te_page &page);
    virtual void OnRead(const struct te_page &page, bool hit) = 0;
    virtual void OnWrite(const struct te_page &page, bool hit) = 0;
    virtual ~SimuCore();

  private:
    std::set<PageTag> cache_; // simulates page cache
};

#endif // SESTET_TRACE_ANALYSER_CORE_H_
