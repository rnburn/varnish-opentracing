#include <lightstep/impl.h>
#include <lightstep/tracer.h>
#include <string>
#include <utility>

extern "C" {
#include <varnish/vrt.h>
}

const std::pair<std::string, const char *> kOpenTracingKeys[] = {
    {"ot-tracer-traceid", "\022ot-tracer-traceid:"},
    {"ot-tracer-spanid", "\021ot-tracer-spanid:"},
    {"ot-tracer-sampled", "\022ot-tracer-sampled:"}};

namespace {
class VarnishHeaderCarrierReader : public lightstep::BasicCarrierReader {
public:
  VarnishHeaderCarrierReader(const vrt_ctx *ctx, gethdr_e where)
      : ctx_{ctx}, where_{where} {}

  void ForeachKey(
      std::function<void(const std::string &, const std::string &)> f) const {
    // Not sure if varnish provides a way to iterate over all the header
    // key-value pairs. See
    //  https://stackoverflow.com/q/44122854
    // so this function only looks up specific headers. This approach has the
    // disadvantage of losing any baggage associated with the context.
    std::string value;
    for (auto &key : kOpenTracingKeys) {
      gethdr_s gethdr = {where_, key.second};
      auto result = VRT_GetHdr(ctx_, &gethdr);
      if (result) {
        value.assign(result);
        f(key.first, value);
      }
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
