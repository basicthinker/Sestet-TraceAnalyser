// Sestet-TraceAnayliser
// Jinglei Ren <jinglei.ren@stanzax.org>
// Nov. 25, 2013

#ifndef SESTET_TRACE_ANALYSER_CORE_H_
#define SESTET_TRACE_ANALYSER_CORE_H_

#include <stdint.h>
#include <set>

typedef std::pair<unsigned long, double> PageTag;

#define PAGE_SIZE 4096

struct te_page { // kernel struct Project Sestet-AdaFS
  char te_type;
  char te_hit;
  uint32_t te_ino;
  uint32_t te_pgi;
};

class SimuCore {
  public:
    SimuCore();
    void Input(char operation, const struct te_page &page);
    virtual void OnRead(const struct te_page &page, bool hit) = 0;
    virtual void OnWrite(const struct te_page &page, bool hit) = 0;

    // Counterparts of kernel definitions
    const char kOpRead = 'r';
    const char kOpWite = 'w';
    const char kOpFsync = 's';
    const char kOpEvict = 'e';

    const char kHitYes = 'y';
    const char kHitNo = 'n';
    const char kHitUnkown = 'u';
 
  private:
    std::set<PageTag> cache_; // simulates page cache
};

#endif // SESTET_TRACE_ANALYSER_CORE_H_
