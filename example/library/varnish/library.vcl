vcl 4.0;

import lightstep;
import opentracing;

sub vcl_init {
  include "lightstep_access_token_params";
  lightstep.component_name("library");
  lightstep.init_tracer();
}

sub vcl_recv {
  opentracing.trace_request();
}

sub vcl_backend_fetch {
  opentracing.trace_backend_request();
}

sub vcl_backend_response {
  set beresp.do_esi = true;
}

backend default {
  .host = "127.0.0.1";
  .port = "3001";
}
