vcl 4.0;

import opentracing;
import lightstep;

sub vcl_init {
  lightstep.access_token("abc123");
  lightstep.init_tracer();
}

backend default {
  .host = "127.0.0.1";
  .port = "3001";
}
