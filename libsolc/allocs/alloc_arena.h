#ifndef __SOLC_ALLOC_ARENA_H__
#define __SOLC_ALLOC_ARENA_H__

#include "solc/defs.h"

typedef struct {
  void *memory;
  uptr end;
  uptr cursor;
} alloc_arena_block_t;

typedef struct {
  alloc_arena_block_t *blocks;
  sz blocks_num;
} alloc_arena_t;

alloc_arena_t alloc_arena_create(void);
void alloc_arena_destroy(alloc_arena_t *alloc_arena);
void *alloc_arena_allocate_aligned(alloc_arena_t *alloc_arena, sz size,
                                   sz alignment);
static inline void *alloc_arena_allocate(alloc_arena_t *alloc_arena, sz size)
{
  return alloc_arena_allocate_aligned(alloc_arena, size, 16);
}
void alloc_arena_clear(alloc_arena_t *alloc_arena);

#endif // __SOLC_ALLOC_ARENA_H__
