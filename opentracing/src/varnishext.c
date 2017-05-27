#include "varnishext.h"

#include <varnish/cache/cache.h>
#include <ctype.h>

static void set_header(struct ot_header* header) {
  const char* first = header->hp->hd[header->index].b;
  const char* last = header->hp->hd[header->index].e;
  const char *key_last, *value_first;
  for (key_last = first; key_last != last; ++key_last)
    if (*key_last == ':') break;
  if (key_last != last)
    value_first = key_last + 1;
  else
    value_first = key_last;
  for (; value_first != last; ++value_first)
    if (!isspace(*value_first)) break;
  header->key_first = first;
  header->key_last = key_last;
  header->value_first = value_first;
  header->value_last = last;
}

struct ot_header ot_header_begin(const struct vrt_ctx* ctx,
                                 enum gethdr_e where) {
  struct ot_header header;
  header.hp = VRT_selecthttp(ctx, where);
  header.index = HTTP_HDR_FIRST;
  set_header(&header);
  return header;
}

int ot_header_is_end(const struct ot_header* header) {
  return header->index == header->hp->nhd;
}

void ot_header_next(struct ot_header* header) {
  ++header->index;
  set_header(header);
}
