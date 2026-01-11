#ifndef __SOLC_CONTAINER_VECTOR_H__
#define __SOLC_CONTAINER_VECTOR_H__

#include <solc/defs.h>

#define vector_create(type) __vector_create(16, sizeof(type))
#define vector_reserve(type, n) __vector_create(n, sizeof(type))
#define vector_destroy(v) __vector_destroy(v)
#define vector_push(v, val)     \
  {                             \
    __auto_type tmp = val;      \
    v = __vector_push(v, &tmp); \
  }
#define vector_pop(v, out) __vector_pop(v, out)

void *__vector_create(sz cap, sz stride);
void __vector_destroy(void *v);
void *__vector_push(void *v, const void *val);
void __vector_pop(void *v, void *out);

sz vector_get_capacity(const void *v);
sz vector_get_stride(const void *v);
sz vector_get_length(const void *v);

void vector_clear(void *v);

#endif // __SOLC_CONTAINER_VECTOR_H__
