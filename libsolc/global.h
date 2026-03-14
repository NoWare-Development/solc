#ifndef __SOLC_GLOBAL_H__
#define __SOLC_GLOBAL_H__

#include "allocs/alloc_arena.h"

void global_init(void);
void global_deinit(void);

extern alloc_arena_t __global_arena_alloc;

static inline alloc_arena_t *global_arena_alloc(void)
{
  return &__global_arena_alloc;
}

#endif // __SOLC_GLOBAL_H__
