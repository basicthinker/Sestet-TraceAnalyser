// versioned_cache.h
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Oct. 6, 2014

#ifndef SESTET_TRACE_ANALYSER_VERSIONED_CACHE_H_
#define SESTET_TRACE_ANALYSER_VERSIONED_CACHE_H_

#include <map>
#include "../data_item.h"

class VersionedCache {
 public:
  VersionedCache() : version_(0) { }
  bool Insert(unsigned long file, unsigned long index);
  bool Erase(unsigned long file, unsigned long index);
  void ClearOldVersions() { ClearOldVersions(version()); }
  void ClearOldVersions(int v);

  unsigned long Size() { return cache_.size(); }
  int version() const { return version_; }
  void IncVersion() { ++version_; }

 private:
  std::multimap<DataTag, int> cache_;
  int version_;
  typedef std::multimap<DataTag, int>::iterator CacheIterator;
};

bool VersionedCache::Insert(unsigned long file, unsigned long index) {
  DataTag tag(file, index);
  std::pair<CacheIterator, CacheIterator> values = cache_.equal_range(tag);
  for (CacheIterator it = values.first; it != values.second; ++it) {
    if (it->second == version()) return true;
  }
  cache_.insert(std::pair<DataTag, int>(tag, version()));
  return false;
}

bool VersionedCache::Erase(unsigned long file, unsigned long index) {
  DataTag tag(file, index);
  std::pair<CacheIterator, CacheIterator> values = cache_.equal_range(tag);
  for (CacheIterator it = values.first; it != values.second; ) {
    if (it->second == version()) {
      cache_.erase(it++);
      return true;
    } else ++it;
  }
  return false;
}

void VersionedCache::ClearOldVersions(int v) {
  for (CacheIterator it = cache_.begin(); it != cache_.end(); ) {
    if (it->second < v) {
      cache_.erase(it++);
    } else ++it;
  }
}

#endif // SESTET_TRACE_ANALYSER_VERSIONED_CACHE_H_

