#include "allocs/alloc_arena.h"
#include "containers/vector.h"
#include "solc/defs.h"
#include <stdlib.h>

#define PAGE_SIZE 4096

static inline alloc_arena_block_t *
alloc_arena_add_block(alloc_arena_t *alloc_arena, sz block_size);
static inline sz get_aligned(sz x, sz alignment);

alloc_arena_t alloc_arena_create(void)
{
  return (alloc_arena_t){
    .blocks = vector_reserve(alloc_arena_block_t, 16),
    .blocks_num = 0,
  };
}

void alloc_arena_destroy(alloc_arena_t *alloc_arena)
{
  SOLC_ASSUME(alloc_arena != nullptr);
  for (sz i = 0; i < alloc_arena->blocks_num; i++)
    free(alloc_arena->blocks[i].memory);
  vector_destroy(alloc_arena->blocks);
}

void *alloc_arena_allocate_aligned(alloc_arena_t *alloc_arena, sz size,
                                   sz alignment)
{
  SOLC_ASSUME(alloc_arena != nullptr && size > 0 && alignment > 0);

  sz real_size = alignment + size;
  alloc_arena_block_t *suitable_block = nullptr;
  if (real_size > PAGE_SIZE)
    goto alloc_arena_allocate_aligned_no_block;

  for (sz i = 0; i < alloc_arena->blocks_num; i++) {
    alloc_arena_block_t *block = &alloc_arena->blocks[i];
    if (block->end - (uptr)block->memory - block->cursor < real_size)
      continue;

    suitable_block = block;
    break;
  }

  if (suitable_block == nullptr)
alloc_arena_allocate_aligned_no_block:
    suitable_block =
      alloc_arena_add_block(alloc_arena, get_aligned(real_size, PAGE_SIZE));

  void *out_data = (void *)get_aligned(
    (uptr)(suitable_block->memory + suitable_block->cursor), alignment);
  suitable_block->cursor += real_size;

  return out_data;
}

void alloc_arena_clear(alloc_arena_t *alloc_arena)
{
  SOLC_ASSUME(alloc_arena != nullptr);
  for (sz i = 0; i < alloc_arena->blocks_num; i++)
    alloc_arena->blocks[i].cursor = (uptr)alloc_arena->blocks[i].memory;
}

static inline alloc_arena_block_t *
alloc_arena_add_block(alloc_arena_t *alloc_arena, sz block_size)
{
  alloc_arena_block_t block = { 0 };
  block.memory = malloc(block_size);
  block.end = (uptr)(block.memory + block_size);
  block.cursor = 0;
  vector_push(alloc_arena->blocks, block);

  alloc_arena_block_t *block_ptr =
    &alloc_arena->blocks[alloc_arena->blocks_num++];
  return block_ptr;
}

static inline sz get_aligned(sz x, sz alignment)
{
  return x + (-x & (alignment - 1));
}
