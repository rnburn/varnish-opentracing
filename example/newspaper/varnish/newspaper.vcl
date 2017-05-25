vcl 4.0;

include "opentracing.vcl";
import lightstep;
import directors;

backend node1 {
  .host = "127.0.0.1";
  .port = "3001";
}

backend node2 {
  .host = "127.0.0.1";
  .port = "3001";
}

backend node3 {
  .host = "127.0.0.1";
  .port = "3001";
}

sub vcl_init {
  new default_director = directors.round_robin();
  default_director.add_backend(node1);
  default_director.add_backend(node2);
  default_director.add_backend(node3);

  lightstep.access_token("${LIGHTSTEP_ACCESS_TOKEN}");
  lightstep.component_name("newspaper");
  lightstep.init_tracer();
}

sub vcl_recv {
  set req.backend_hint = default_director.backend();

  opentracing.trace_request();
  if (req.url ~ "^/article") {
    opentracing.operation_name("article");
  } else {
    opentracing.operation_name(req.url);
  }
  return (hash);
}

sub vcl_hash {
  hash_data(req.url);
  return (lookup);
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
