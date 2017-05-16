#pragma once

#include <unordered_map>
#include <mutex>
#include <lightstep/tracer.h>

class SpanMap {
 public:
  void insert(void* key, lightstep::Span* span);
  lightstep::Span* lookup_span(void* key);
  void erase(void* key);

 private:
  std::mutex mutex_;
  std::unordered_map<void*, lightstep::Span*> spans_;
};
