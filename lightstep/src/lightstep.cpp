#include <lightstep/impl.h>
#include <lightstep/recorder.h>
#include <iostream>
#include <memory>

extern "C" {
#include <varnish/vcl.h>
#include <varnish/vdef.h>
#include <varnish/vrt.h>
}

static lightstep::TracerOptions *make_default_tracer_options() {
  std::unique_ptr<lightstep::TracerOptions> tracer_options{
      new lightstep::TracerOptions{}};
  tracer_options->collector_host = "collector-grpc.lightstep.com";
  tracer_options->collector_port = 443;
  tracer_options->collector_encryption = "tls";
  return tracer_options.release();
}

extern "C" int lightstep_event(VRT_CTX, struct vmod_priv *lightstep_context,
                               enum vcl_event_e event) {
  if (event == VCL_EVENT_LOAD) {
    auto tracer_options = make_default_tracer_options();
    lightstep_context->priv = tracer_options;
    lightstep_context->free = [](void *tracer_options) {
      delete static_cast<lightstep::TracerOptions *>(tracer_options);
    };
  }
  return 0;
}

extern "C" VCL_VOID vmod_collector_host(VRT_CTX,
                                        struct vmod_priv *lightstep_context,
                                        VCL_STRING collector_host) {
  auto tracer_options =
      static_cast<lightstep::TracerOptions *>(lightstep_context->priv);
  tracer_options->collector_host = collector_host;
}

extern "C" VCL_VOID vmod_collector_encryption(
    VRT_CTX, struct vmod_priv *lightstep_context,
    VCL_STRING collector_encryption) {
  auto tracer_options =
      static_cast<lightstep::TracerOptions *>(lightstep_context->priv);
  tracer_options->collector_encryption = collector_encryption;
}

extern "C" VCL_VOID vmod_collector_port(VRT_CTX,
                                        struct vmod_priv *lightstep_context,
                                        VCL_INT collector_port) {
  auto tracer_options =
      static_cast<lightstep::TracerOptions *>(lightstep_context->priv);
  tracer_options->collector_port = collector_port;
}

extern "C" VCL_VOID vmod_component_name(VRT_CTX,
                                        struct vmod_priv *lightstep_context,
                                        VCL_STRING component_name) {
  auto tracer_options =
      static_cast<lightstep::TracerOptions *>(lightstep_context->priv);
  tracer_options->tracer_attributes["lightstep.component_name"] =
      component_name;
}

extern "C" VCL_VOID vmod_access_token(VRT_CTX,
                                      struct vmod_priv *lightstep_context,
                                      VCL_STRING access_token) {
  auto tracer_options =
      static_cast<lightstep::TracerOptions *>(lightstep_context->priv);
  tracer_options->access_token = access_token;
}

extern "C" VCL_VOID vmod_init_tracer(VRT_CTX,
                                     struct vmod_priv *lightstep_context) {
  auto tracer_options =
      static_cast<lightstep::TracerOptions *>(lightstep_context->priv);
  lightstep::BasicRecorderOptions recorder_options;
  lightstep::Tracer::InitGlobal(
      NewLightStepTracer(*tracer_options, recorder_options));
}
