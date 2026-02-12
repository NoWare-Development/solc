#include "containers/hashset.h"
#include "hash.h"
#include "solc/defs.h"
#include "types.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define HS_MAX_LOAD_FACTOR 0.65f
#define HS_INITIAL_SIZE 128
#define HS_RESIZE_FACTOR 2

typedef u8 hs_ctrl_t;
typedef enum {
  HS_CTRL_FLAG_EMPTY = 0b10000000,
  HS_CTRL_FLAG_DELETED = 0b11111111,
} hs_ctrl_flags_t;

typedef struct __hashset_t {
  hs_ctrl_t *ctrl;
  void *key_slots;

  sz size;
  sz filled;

  sz key_size;

  hash_function_t hash_function;
  get_size_function_t get_key_size_function;
  compare_function_t key_compare_function;

  size_policy_t key_size_policy;
} hashset_t;

static inline hash_t hs_h1(hash_t hash);
static inline hash_t hs_h2(hash_t hash);
static inline b8 hs_ctrl_flag_is_present(hs_ctrl_t ctrl);
static hashset_t *hs_resize_and_rehash(hashset_t *set);
static inline void *hs_get_key_slot_addr(hashset_t *set, sz pos);
static void hs_set_key(hashset_t *set, sz pos, const void *key);

#define CTRL_BLOCK_SIZE(_size) (sizeof(hs_ctrl_t) * (_size))
#define KEY_BLOCK_SIZE(_key_size, _size) ((_key_size) * (_size))
#define FULL_BLOCK_SIZE(_key_size, _size) \
  (CTRL_BLOCK_SIZE((_size)) + KEY_BLOCK_SIZE((_key_size), (_size)))

hashset_t *__hashset_create_impl(sz key_size, hash_function_t hash_function,
                                 size_policy_t key_size_policy,
                                 get_size_function_t get_key_size_function,
                                 compare_function_t key_compare_function)
{
  SOLC_ASSUME(hash_function != nullptr);

  hashset_t *out_set = malloc(sizeof(hashset_t));

  if (key_size_policy == SIZE_POLICY_VARIABLE)
    key_size = sizeof(void *);

  void *block = malloc(FULL_BLOCK_SIZE(key_size, HS_INITIAL_SIZE));

  out_set->ctrl = block;
  memset(out_set->ctrl, HS_CTRL_FLAG_EMPTY, CTRL_BLOCK_SIZE(HS_INITIAL_SIZE));

  out_set->key_slots = (char *)block + CTRL_BLOCK_SIZE(HS_INITIAL_SIZE);
  memset(out_set->key_slots, 0, KEY_BLOCK_SIZE(key_size, HS_INITIAL_SIZE));

  out_set->size = HS_INITIAL_SIZE;
  out_set->filled = 0;
  out_set->key_size = key_size;
  out_set->hash_function = hash_function;
  out_set->get_key_size_function = get_key_size_function;
  out_set->key_compare_function = key_compare_function;
  out_set->key_size_policy = key_size_policy;

  return out_set;
}

void hashset_destroy(hashset_t *set)
{
  SOLC_ASSUME(set != nullptr);

  if (set->key_size_policy == SIZE_POLICY_VARIABLE) {
    for (sz i = 0; i < set->size; i++) {
      if (hs_ctrl_flag_is_present(set->ctrl[i]))
        continue;

      void *key = hs_get_key_slot_addr(set, i);
      key = *(void **)key;
      free(key);
    }
  }

  free(set->ctrl);
  free(set);
}

hashset_t *__hashset_set_impl(hashset_t *set, const void *key)
{
  SOLC_ASSUME(set != nullptr && key != nullptr);

  hash_t hash = set->hash_function(key);
  sz pos = hs_h1(hash) % set->size;
  hs_ctrl_t h2 = hs_h2(hash);

  while (1) {
    if (h2 == set->ctrl[pos]) {
      void *slot_key = hs_get_key_slot_addr(set, pos);
      if (set->key_size_policy == SIZE_POLICY_VARIABLE) {
        if SOLC_UNLIKELY (set->key_compare_function == nullptr)
          SOLC_NOREACH();

        slot_key = *(void **)slot_key;
        SOLC_ASSUME(slot_key != nullptr);

        if SOLC_LIKELY (set->key_compare_function(key, slot_key)) {
          return set;
        }
      } else if SOLC_LIKELY (memcmp(key, slot_key, set->key_size) == 0) {
        return set;
      }
    }

    if (hs_ctrl_flag_is_present(set->ctrl[pos])) {
      hs_set_key(set, pos, key);

      set->ctrl[pos] = h2;
      set->filled++;

      float load_factor = (float)set->filled / set->size;
      if SOLC_UNLIKELY (load_factor > HS_MAX_LOAD_FACTOR) {
        set = hs_resize_and_rehash(set);
      }

      return set;
    }

    pos = (pos + 1) % set->size;
  }
}

b8 __hashset_is_set_impl(hashset_t *set, const void *key)
{
  SOLC_ASSUME(set != nullptr && key != nullptr);

  hash_t hash = set->hash_function(key);
  sz pos = hs_h1(hash) % set->size;
  hs_ctrl_t h2 = hs_h2(hash);

  while (1) {
    if (h2 == set->ctrl[pos]) {
      void *slot_key = hs_get_key_slot_addr(set, pos);
      if (set->key_size_policy == SIZE_POLICY_VARIABLE) {
        if SOLC_UNLIKELY (set->key_compare_function == nullptr)
          SOLC_NOREACH();

        slot_key = *(void **)slot_key;
        SOLC_ASSUME(slot_key != nullptr);

        if SOLC_LIKELY (set->key_compare_function(key, slot_key)) {
          return true;
        }
      } else if SOLC_LIKELY (memcmp(key, slot_key, set->key_size) == 0) {
        return true;
      }
    }

    if SOLC_UNLIKELY (set->ctrl[pos] == HS_CTRL_FLAG_EMPTY)
      return false;

    pos = (pos + 1) % set->size;
  }
}

void __hashset_unset_impl(hashset_t *set, const void *key)
{
  SOLC_ASSUME(set != nullptr && key != nullptr);

  hash_t hash = set->hash_function(key);
  sz pos = hs_h1(hash) % set->size;

  while (1) {
    if (hs_h2(hash) == set->ctrl[pos]) {
      void *slot_key = hs_get_key_slot_addr(set, pos);
      if (set->key_size_policy == SIZE_POLICY_VARIABLE) {
        SOLC_ASSUME(set->key_compare_function != nullptr);
        if SOLC_LIKELY (set->key_compare_function(key, *(void **)slot_key)) {
          void **key_slot_ptr = slot_key;

          free(*key_slot_ptr);
          *key_slot_ptr = 0;

          set->ctrl[pos] = HS_CTRL_FLAG_DELETED;
          set->filled--;
          return;
        }
      } else if SOLC_LIKELY (memcmp(key, slot_key, set->key_size) == 0) {
        memset(slot_key, 0, set->key_size);
        set->ctrl[pos] = HS_CTRL_FLAG_DELETED;
        set->filled--;
        return;
      }
    }

    if (set->ctrl[pos] == HS_CTRL_FLAG_EMPTY)
      return;

    pos = (pos + 1) % set->size;
  }
}

sz hashset_get_size(hashset_t *set)
{
  SOLC_ASSUME(set != nullptr);
  return set->filled;
}

b8 hashset_is_empty(hashset_t *set)
{
  SOLC_ASSUME(set != nullptr);
  return set->filled == 0;
}

static inline hash_t hs_h1(hash_t hash)
{
  return hash >> 7;
}

static inline hash_t hs_h2(hash_t hash)
{
  return hash & 0x7F;
}

static inline b8 hs_ctrl_flag_is_present(hs_ctrl_t ctrl)
{
  return (ctrl & 0b10000000) != 0;
}

static hashset_t *hs_resize_and_rehash(hashset_t *set)
{
  SOLC_ASSUME(set != nullptr);

  sz new_size = set->size * HS_RESIZE_FACTOR;

  sz new_memreq = FULL_BLOCK_SIZE(set->key_size, new_size);
  void *new_block = malloc(new_memreq);

  const sz ctrl_block_size = CTRL_BLOCK_SIZE(new_size);

  hs_ctrl_t *new_ctrl = new_block;
  memset(new_ctrl, HS_CTRL_FLAG_EMPTY, ctrl_block_size);

  void *new_key_slots = (char *)new_block + ctrl_block_size;
  memset(new_key_slots, 0, KEY_BLOCK_SIZE(set->key_size, new_size));

  hashset_t *new_set = malloc(sizeof(hashset_t));
  memcpy(new_set, set, sizeof(hashset_t));
  new_set->ctrl = new_ctrl;
  new_set->key_slots = new_key_slots;
  new_set->size = new_size;
  new_set->filled = 0;

  for (sz i = 0; i < set->size; i++) {
    if (hs_ctrl_flag_is_present(set->ctrl[i]))
      continue;

    void *key = hs_get_key_slot_addr(set, i);

    if (set->key_size_policy == SIZE_POLICY_VARIABLE)
      key = *(void **)key;

    __hashset_set_impl(new_set, key);
  }

  return new_set;
}

static inline void *hs_get_key_slot_addr(hashset_t *set, sz pos)
{
  SOLC_ASSUME(set != nullptr);
  return (char *)set->key_slots + (set->key_size * pos);
}

static void hs_set_key(hashset_t *set, sz pos, const void *key)
{
  SOLC_ASSUME(set != nullptr && key != nullptr);
  void *key_slot = hs_get_key_slot_addr(set, pos);

  if (set->key_size_policy == SIZE_POLICY_VARIABLE) {
    void **key_ptr = key_slot;
    if (*key_ptr != nullptr)
      free(*key_ptr);

    sz key_size = set->get_key_size_function(key);
    *key_ptr = malloc(key_size);
    memcpy(*key_ptr, key, key_size);

    return;
  }

  memcpy(key_slot, key, set->key_size);
}
