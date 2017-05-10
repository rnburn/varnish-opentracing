vcl 4.0;

import opentracing;

sub vcl_init {
  opentracing.init_tracer("abc123");
}

backend default {
  .host = "127.0.0.1";
  .port = "3001";
}
