#pragma once

/**
 * Many varnish header files lack include guards. This files wraps the common
 * headers used within an include guard to prevent multiple definition errors.
 */

#ifdef __cplusplus
extern "C" {
#endif
#include <varnish/vrt.h>
#ifdef __cplusplus
} // extern "C"
#endif
