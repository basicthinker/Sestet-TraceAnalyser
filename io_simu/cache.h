// cache.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Oct. 4, 2014

#ifndef SESTET_TRACE_ANALYSER_CACHE_H_
#define SESTET_TRACE_ANALYSER_CACHE_H_

#include <set>
#include "data_item.h"

class Cache {
 public:
  bool Insert(unsigned long file, unsigned long index);
  bool Erase(unsigned long file, unsigned long index);
  int Erase(unsigned long file);
  void Clear() { cache_.clear(); }
  unsigned long Size() const { return cache_.size(); }
 private:
  std::set<DataTag> cache_;
};

bool Cache::Insert(unsigned long file, unsigned long index) {
  DataTag tag(file, index);
  if (cache_.count(tag)) return true;
  else {
    cache_.insert(tag);
    return false;
  }
}

bool Cache::Erase(unsigned long file, unsigned long index) {
  DataTag tag(file, index);
  if (!cache_.count(tag)) return false;
  else {
    cache_.erase(tag);
    return true;
  }
}

int Cache::Erase(unsigned long file) {
  int num = 0;
  for (std::set<DataTag>::iterator it = cache_.begin();
      it != cache_.end();) {
    if (it->first == file) {
      it = cache_.erase(it);
      ++num;
    } else ++it;
  }
  return num;
}

#endif // SESTET_TRACE_ANALYSER_CACHE_H_

