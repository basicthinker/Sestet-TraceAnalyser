// trace_parser.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 26, 2013

#ifndef SESTET_TRACE_ANALYSER_PARSER_H_
#define SESTET_TRACE_ANALYSER_PARSER_H_

#include <cerrno>
#include <fstream>
#include "data_item.h"

// Kernel counterparts
#include <stdint.h>

struct timeval32 {
  uint32_t tv_sec;
  uint32_t tv_usec;
};

inline double tv_float(const struct timeval32 &tv) {
  return tv.tv_sec + (double)tv.tv_usec / 1000000;
}

struct te_page {
  char te_op;
  char te_hit;
  uint32_t te_ino;
  uint32_t te_pgi;
};

class TraceParser {
  public:
    TraceParser() : last_time_(0) { }
    ~TraceParser() { this->Close(); }
    bool Open(const char *file_name);
    bool Next(DataOperation &op, DataItem &item);
    void Close() { input_.close(); }
  private:
    std::ifstream input_;
    double last_time_;
};

bool TraceParser::Open(const char *file_name) {
  if (input_.is_open()) this->Close();
  input_.open(file_name);
  return input_.is_open();
}

bool TraceParser::Next(DataOperation &op, DataItem &item) {
  if (!input_.is_open()) return false;
  struct timeval32 tv;
  struct te_page page;
  if (!input_.read((char *)&tv, sizeof(tv)) ||
      !input_.read((char *)&page, sizeof(page))) {
    return false;
  }

  item.di_time = tv_float(tv);
  if (item.di_time < last_time_) {
    item.di_time = last_time_; // Times not guaranteed to strictly increase
  }
  last_time_ = item.di_time;

  item.di_file = page.te_ino;
  item.di_index = page.te_pgi;
  op = (DataOperation)page.te_op;
  return true;
}

#endif // SESTET_TRACE_ANALYSER_PARSER_H_

