#include "varnish.h"
#include <lightstep/impl.h>
#include <lightstep/tracer.h>
#include <algorithm>
#include <iostream>

namespace {
class VarnishHeaderCarrierWriter : public lightstep::BasicCarrierWriter {
 public:
  VarnishHeaderCarrierWriter(gethdr_e where, const vrt_ctx* ctx)
      : where_{where}, ctx_{ctx} {}

  void Set(const std::string& key, const std::string& value) const override {
    key_.resize(key.size() + 2);
    std::copy(key.begin(), key.end(), std::next(key_.begin()));
    key_[0] = static_cast<char>(key.size()+1);
    key_.back() = ':';
    gethdr_s gethdr = {where_, key_.data()};
    VRT_SetHdr(ctx_, &gethdr, value.data(), vrt_magic_string_end);
  }

 private:
  gethdr_e where_;
  const vrt_ctx* ctx_;
  mutable std::string key_;
};
}

void inject_span_context(lightstep::Tracer& tracer, const vrt_ctx* ctx,
                         gethdr_e where,
                         const lightstep::SpanContext& span_context) {
  auto carrier_writer = VarnishHeaderCarrierWriter{where, ctx};
  auto was_successful = tracer.Inject(
      span_context, lightstep::CarrierFormat::HTTPHeaders, carrier_writer);
  if (!was_successful) std::cerr << "Error: Could not inject span context!\n";
}
