OpenTracing in Varnish
----------------------

I think it's possible to enable OpenTracing in Varnish using a combination of
[modules](https://varnish-cache.org/docs/trunk/reference/vmod.html), Varnish
Configuration Language (VCL) code, and monitoring of the shared memory log.
Unlike nginx, Varnish modules don't provide much capability to set up callbacks
that get executed when certain events around an HTTP request occur. Instead,
their main purpose is to expose C functions that you can call from within VCL;
but VCL allows you to set up event-driven code, so you could write both a
Varnish C++ module that links to the LightStep C++ library and manages spans,
and an OpenTracing VCL "library" (say opentracing.vcl) that a user would
[include](http://www.varnish-cache.org/docs/4.0/reference/vcl.html#include-statement)
in their varnish config.

In VCL, you execute event driven code by defining certain built-in subroutines.
For example, you could write

```vcl
sub vcl_recv {
    # Execute some code.
}
```

to execute commands whenever a request is first received. VCL also permits you to
define the subroutine multiple times,
[combing](http://www.varnish-cache.org/docs/4.0/reference/vcl.html#multiple-subroutines)
the commands from all the definitions, so if someone wanted to enable OpenTracing
I could see them doing something like the following:

```vcl
include "opentracing.vcl";

sub vcl_recv {
  # `opentracing_trace_request` is a function exposed by the Varnish C++ module.
  # It enables tracing for the given request. In the C++ code, this would 
  # allocate memory to keep track of the request's span and inject it's context
  # into the request headers.
  opentracing_trace_request(req);

  # Additionally, the module would also expose commands to manipulate span tags
  # and log data.
  opentracing_tag(req, "abc", "123");
}
```

`opentracing.vcl` would hook into many of the VCL [built-in
subroutines](https://varnish-cache.org/docs/trunk/users-guide/vcl-built-in-subs.html),
defining code that gets conditionally executed if OpenTracing is enabled. It
might look something like this:

```vcl
# Loads the C++ OpenTracing module.
import opentracing;

sub vcl_backend_fetch {
  # Varnish is calling a backend, so instruct the C++ module to 
  # create a child span for this.
  if (opentracing_enabled(req)) {
    opentracing_trace_backend(req, bereq);
  }
}

sub vcl_backend_response {
  # The backend returned, so finish its span.
  if (opentracing_enabled(req)) {
    opentracing_finish_backend_span(req, bereq, beresp);
  }
}

sub vcl_backend_error {
  # Finish the backend span with an error.
  ...
}

sub vcl_hit {
  # Set some tags to indicate that Varnish was able to return a cached response.
}
...
```

There's no subroutine from VCL that lets you invoke a module function when a
request is finished, so use either a [VMOD
PRIV_TASK](https://www.varnish-cache.org/lists/pipermail/varnish-misc/2017-May/025891.html)
or set up a
[callback](https://www.varnish-cache.org/lists/pipermail/varnish-misc/2017-May/025895.html)
on the shared memory log to finish the request spans.
