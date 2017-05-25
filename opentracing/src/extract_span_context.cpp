#include <lightstep/impl.h>
#include <lightstep/tracer.h>
#include <string>
#include <utility>
#include "varnish.h"

#include "varnishext.h"

namespace {
class VarnishHeaderCarrierReader : public lightstep::BasicCarrierReader {
public:
  VarnishHeaderCarrierReader(const vrt_ctx *ctx, gethdr_e where)
      : ctx_{ctx}, where_{where} {}

  void ForeachKey(
      std::function<void(const std::string &, const std::string &)> f) const {
    std::string key, value;
    for (auto header = ot_header_begin(ctx_, where_);
         !ot_header_is_end(&header); ot_header_next(&header)) {
      key.assign(
          header.key_first,
          std::distance(header.key_first, header.key_last));
      value.assign(
          header.value_first,
          std::distance(header.value_first, header.value_last));
      f(key, value);
    }
  }

private:
  const vrt_ctx *ctx_;
  gethdr_e where_;
};
}

lightstep::SpanContext extract_span_context(lightstep::Tracer &tracer,
                                            const vrt_ctx *ctx,
                                            gethdr_e where) {
  auto carrier_reader = VarnishHeaderCarrierReader{ctx, where};
  auto span_context =
      tracer.Extract(lightstep::CarrierFormat::HTTPHeaders, carrier_reader);
  return span_context;
}
