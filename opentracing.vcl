import opentracing;

sub vcl_backend_fetch {
  opentracing.trace_backend_request();
}

sub vcl_backend_response {
  opentracing.status_code(beresp.status);
}

sub vcl_backend_error {
  opentracing.status_code(beresp.status);
}

sub vcl_deliver {
  opentracing.status_code(resp.status);
}

sub vcl_synth {
  opentracing.status_code(resp.status);
}

sub vcl_hit {
  opentracing.tag("varnish.cache", "hit");
}

sub vcl_miss {
  opentracing.tag("varnish.cache", "miss");
}
