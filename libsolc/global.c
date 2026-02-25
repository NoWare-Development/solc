#include "global.h"
#include "allocs/alloc_arena.h"

static alloc_arena_t alloc_arena = { 0 };
b8 initialized = false;

void global_init(void)
{
  SOLC_ASSUME(!initialized); // FIXME: Replace it with assert.

  alloc_arena = alloc_arena_create();

  initialized = true;
}

void global_deinit(void)
{
  SOLC_ASSUME(initialized); // FIXME: Replace it with assert.

  alloc_arena_destroy(&alloc_arena);

  initialized = false;
}

alloc_arena_t *global_arena_alloc(void)
{
  return &alloc_arena;
}
