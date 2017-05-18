import opentracing;

sub vcl_recv {
  # This header is used internally to pass context. Unset so that a malicious
  # client can't cause trouble by passing in invalid values.
  unset req.http.ot-varnish-context;
}

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
