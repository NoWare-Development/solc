#ifndef __SOLC_CONTAINERS_ARRAY_H__
#define __SOLC_CONTAINERS_ARRAY_H__

#include "solc/defs.h"

#define array_create(_type, _size) __array_create_impl(sizeof(_type), (_size))
#define array_create_z(_type, _size) \
  __array_create_z_impl(sizeof(_type), (_size))

void *__array_create_impl(sz stride, sz capacity);
void *__array_create_impl_z(sz stride, sz capacity);

#endif // __SOLC_CONTAINERS_ARRAY_H__
