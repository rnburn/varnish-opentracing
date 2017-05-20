vcl 4.0;

include "opentracing.vcl";
import lightstep;

sub vcl_init {
  lightstep.access_token("${LIGHTSTEP_ACCESS_TOKEN}");
  lightstep.component_name("newspaper");
  lightstep.init_tracer();
}

sub vcl_recv {
  opentracing.trace_request();
  if (req.url ~ "^/article") {
    opentracing.operation_name("/article");
  } else {
    opentracing.operation_name(req.url);
  }
  return (hash);
}

sub vcl_backend_fetch {
  if (bereq.url ~ "^/article") {
    opentracing.operation_name("article");
  } else {
    opentracing.operation_name(bereq.url);
  }
}

sub vcl_backend_response {
  if (bereq.url ~ "^/article") {
    set beresp.ttl = 1d;
  } else {
    set beresp.ttl = 5s;
  }
  set beresp.do_esi = true;
}

backend default {
  .host = "127.0.0.1";
  .port = "3001";
}
