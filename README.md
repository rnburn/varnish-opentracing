varnish-opentracing
-----------------

Enable requests served by varnish for distributed tracing via [The OpenTracing Project](opentracing.io).

Dependencies
------------
- [LightStep's C++ tracer](https://github.com/lightstep/lightstep-tracer-cpp).  
- A LightStep [account](http://lightstep.com/#request-access).
- [Varnish](http://nginx.org/).

Building
--------
```
$ git clone git@github.com:rnburn/varnish-opentracing.git
$ cd varnish-opentracing
$ mkdir .build
$ cmake ..
$ make && sudo make install
```
