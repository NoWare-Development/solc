#include "containers/vector.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  sz capacity;
  sz stride;
  sz len;
} vector_header_t;

static inline vector_header_t *get_vector_header(const void *v);
static inline void *vector_resize(void *v);

void *__vector_create(sz cap, sz stride)
{
  void *v = malloc(sizeof(vector_header_t) + (cap * stride));
  vector_header_t *header = v;

  header->capacity = cap;
  header->stride = stride;
  header->len = 0;

  v += sizeof(vector_header_t);
  return v;
}

void __vector_destroy(void *v)
{
  v -= sizeof(vector_header_t);
  free(v);
}

void *__vector_push(void *v, const void *val)
{
  vector_header_t *header = get_vector_header(v);
  if (header->len >= header->capacity) {
    v = vector_resize(v);
    header = get_vector_header(v);
  }

  memcpy(v + (header->len * header->stride), val, header->stride);
  header->len++;

  return v;
}

void __vector_pop(void *v, void *out)
{
  if (vector_get_length(v) == 0) {
    return;
  }

  vector_header_t *header = get_vector_header(v);
  memcpy(out, v + ((header->len - 1) * header->stride), header->stride);
  header->len--;
}

sz vector_get_capacity(const void *v)
{
  return get_vector_header(v)->capacity;
}

sz vector_get_stride(const void *v)
{
  return get_vector_header(v)->stride;
}

sz vector_get_length(const void *v)
{
  return get_vector_header(v)->len;
}

void vector_clear(void *v)
{
  vector_header_t *header = v;
  header->len = 0;
}

static inline vector_header_t *get_vector_header(const void *v)
{
  return (vector_header_t *)(v - sizeof(vector_header_t));
}

static inline void *vector_resize(void *v)
{
  vector_header_t *old_header = get_vector_header(v);

  void *newv = __vector_create(old_header->capacity * 2, old_header->stride);
  memcpy(newv, v, old_header->capacity * old_header->stride);
  vector_header_t *newv_header = get_vector_header(newv);
  newv_header->len = old_header->len;

  vector_destroy(v);
  return newv;
}
