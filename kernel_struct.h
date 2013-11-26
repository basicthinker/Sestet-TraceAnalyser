// kernel_struct.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei.ren@gmail.com>
// Nov. 26, 2013

#ifndef SESTET_TRACE_ANALYSER_KERNEL_STRUCT_H_
#define SESTET_TRACE_ANALYSER_KERNEL_STRUCT_H_

#include <stdint.h>

struct timeval32 {
  uint32_t tv_sec;
  uint32_t tv_usec;
};

inline double tv_float(const struct timeval32 &tv) {
  return tv.tv_sec + (double)tv.tv_usec / 1000000;
}

struct te_page {
  char te_type;
  char te_hit;
  uint32_t te_ino;
  uint32_t te_pgi;
};

#define PAGE_SIZE 4096

#define TE_OP_READ  'r'
#define TE_OP_WRITE 'w'
#define TE_OP_FSYNC 's'
#define TE_OP_EVICT 'e'

#define TE_HIT_YES     'y'
#define TE_HIT_NO      'n'
#define TE_HIT_UNKNOWN 'u'

#endif // SESTET_TRACE_ANALYSER_KERNEL_STRUCT_H_

