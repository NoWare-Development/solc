#include "containers/array.h"
#include "allocs/alloc_arena.h"
#include "global.h"
#include <string.h>

void *__array_create_impl(sz stride, sz capacity)
{
  return alloc_arena_allocate(global_arena_alloc(), stride * capacity);
}

void *__array_create_impl_z(sz stride, sz capacity)
{
  void *arr = __array_create_impl(stride, capacity);
  memset(arr, 0, stride * capacity);
  return arr;
}
