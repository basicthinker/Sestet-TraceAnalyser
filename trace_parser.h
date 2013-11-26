// trace_parser.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei.ren@gmail.com>
// Nov. 26, 2013

#ifndef SESTET_TRACE_ANALYSER_PARSER_H_
#define SESTET_TRACE_ANALYSER_PARSER_H_

#include <cerrno>
#include <fstream>
#include "kernel_struct.h"

class TraceParser {
  public:
    TraceParser() {}
    TraceParser(const char *file_name) { this->Open(file_name); }
    bool Open(const char *file_name);
    double Next(struct te_page *page);
    void Close() { input_.close(); }
  private:
    std::ifstream input_;
};

bool TraceParser::Open(const char *file_name) {
  if (input_.is_open()) this->Close();
  input_.open(file_name);
  return input_.is_open();
}

double TraceParser::Next(struct te_page *page) {
  if (!input_.is_open()) return -EBADF;
  struct timeval32 tv;
  if (!input_.read((char *)&tv, sizeof(tv)) ||
      !input_.read((char *)page, sizeof(page))) {
    return -EIO;
  }
  return tv_float(tv);
}

#endif // SESTET_TRACE_ANALYSER_PARSER_H_
