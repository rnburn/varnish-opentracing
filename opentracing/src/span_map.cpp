#include "span_map.h"
#include <cassert>

void SpanMap::insert(void* key, lightstep::Span* span) {
  std::lock_guard<std::mutex> lock_guard{mutex_};
  auto was_inserted = spans_.emplace(key, span);
  assert(was_inserted.second);
}

lightstep::Span* SpanMap::lookup_span(void* key) {
  std::lock_guard<std::mutex> lock_guard{mutex_};
  auto iter = spans_.find(key);
  if (iter != std::end(spans_))
    return iter->second;
  else
    return nullptr;
}

void SpanMap::erase(void* key) {
  std::lock_guard<std::mutex> lock_guard{mutex_};
  auto num_erased = spans_.erase(key);
  assert(num_erased == 1);
}
