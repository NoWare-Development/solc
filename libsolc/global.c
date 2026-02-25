#include "global.h"
#include "allocs/alloc_arena.h"

alloc_arena_t __global_arena_alloc;
b8 initialized = false;

void global_init(void)
{
  SOLC_ASSUME(!initialized); // FIXME: Replace it with assert.

  __global_arena_alloc = alloc_arena_create();

  initialized = true;
}

void global_deinit(void)
{
  SOLC_ASSUME(initialized); // FIXME: Replace it with assert.

  alloc_arena_destroy(&__global_arena_alloc);

  initialized = false;
}
