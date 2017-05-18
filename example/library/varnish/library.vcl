vcl 4.0;

include "opentracing.vcl";
import lightstep;

sub vcl_init {
  lightstep.access_token("${LIGHTSTEP_ACCESS_TOKEN}");
  lightstep.component_name("library");
  lightstep.init_tracer();
}

sub vcl_recv {
  opentracing.trace_request();
}

sub vcl_backend_response {
  set beresp.do_esi = true;
}

backend default {
  .host = "127.0.0.1";
  .port = "3001";
}
