// simu_core.h
// Sestet-TraceAnayliser
//
// Jinglei Ren <jinglei.ren@stanzax.org>
// Nov. 25, 2013

#ifndef SESTET_TRACE_ANALYSER_CORE_H_
#define SESTET_TRACE_ANALYSER_CORE_H_

#include <iostream>
#include <set>

#include "kernel_struct.h"

typedef std::pair<unsigned long, double> PageTag;

class SimuCore {
  public:
    void Input(const struct te_page &page);
    virtual void Clear() { cache_.clear(); }
    virtual ~SimuCore();
  protected:
    virtual void OnRead(const struct te_page &page, bool hit) = 0;
    virtual void OnWrite(const struct te_page &page, bool hit) = 0;
    virtual void OnEvict(const struct te_page &page, bool hit) = 0;
    virtual bool OnFsync(uint32_t ino) = 0;
  private:
    std::set<PageTag> cache_; // simulates page cache
};

// Implementations

using std::cerr;
using std::endl;

void SimuCore::Input(const struct te_page &page) {
  PageTag tag(page.te_ino, page.te_pgi);

  switch (page.te_op) {
  case TE_OP_READ:
    if (cache_.find(tag) != cache_.end()) {
      this->OnRead(page, 1);
    } else {
      cache_.insert(tag);
      this->OnRead(page, 0);
    }
    break;
  case TE_OP_WRITE:
    if (cache_.find(tag) != cache_.end()) {
      this->OnWrite(page, 1);
    } else {
      cache_.insert(tag);
      this->OnWrite(page, 0);
    }
    break;
  case TE_OP_FSYNC:
    if (page.te_pgi != 0) { // integrity check
      cerr << "Warning: integrity check fails:"
          " fsync entry has non-zero page index." << endl;
      break;
    }
    if (this->OnFsync(page.te_ino)) {
      for (std::set<PageTag>::iterator it = cache_.begin();
          it != cache_.end(); ) {
        if (it->first == page.te_ino) cache_.erase(it++);
        else ++it;
      }
    }
    break;
  case TE_OP_EVICT:
    if (cache_.find(tag) != cache_.end()) {
      cache_.erase(tag);
      this->OnEvict(page, 1);
    } else {
      this->OnEvict(page, 0);
    } 
    break;
  defalt:
    cerr << "Warning: invalid trace entry type: " << page.te_op << endl;
  }
}

#endif // SESTET_TRACE_ANALYSER_CORE_H_
