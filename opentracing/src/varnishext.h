#pragma once

#include "varnish.h"

/**
 * varnish-cache header files use some C++ keywords as identifiers, so they
 * can't be included from C++ code. This file defines some C functions that can
 * be used from C++ code to interact with varnish-cache.
 */

#ifdef __cplusplus
extern "C" {
#endif

struct ot_header {
  const char *key_first, *key_last, *value_first, *value_last;
  unsigned index;
  const struct http* hp;
};

struct ot_header ot_header_begin(const struct vrt_ctx* ctx,
                                 enum gethdr_e where);

int ot_header_is_end(const struct ot_header* header);

void ot_header_next(struct ot_header* header);

#ifdef __cplusplus
} // extern "C"
#endif
