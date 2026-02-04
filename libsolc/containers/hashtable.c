#include "containers/hashtable.h"
#include <stdlib.h>
#include <string.h>
#include "solc/defs.h"

#define HT_INITIAL_SIZE 64
#define HT_RESIZE_FACTOR 2
#define HT_MAX_LOAD_FACTOR 0.65f

#define CTRL_BLOCK_SIZE(_num_of_entries) (sizeof(ht_ctrl_t) * (_num_of_entries))
#define KEY_BLOCK_SIZE(_key_size, _num_of_entries) \
  ((_key_size) * (_num_of_entries))
#define VALUE_BLOCK_SIZE(_value_size, _num_of_entries) \
  ((_value_size) * (_num_of_entries))
#define FULL_BLOCK_SIZE(_key_size, _value_size, _num_of_entries) \
  (CTRL_BLOCK_SIZE((_num_of_entries)) +                          \
   KEY_BLOCK_SIZE((_key_size), (_num_of_entries)) +              \
   VALUE_BLOCK_SIZE((_value_size), (_num_of_entries)))

typedef u8 ht_ctrl_t;
typedef enum {
  HT_CTRL_FLAG_EMPTY = 0b10000000,
  HT_CTRL_FLAG_DELETED = 0b11111111,
} ht_ctrl_flags_t;

typedef struct __hashtable_t {
  ht_ctrl_t *ctrl;
  void *slots;

  sz size;
  sz filled;

  sz key_size;
  sz value_size;

  hash_function_t hash_function;
  get_size_function_t get_key_size_function, get_value_size_function;
  compare_function_t key_compare_function;

  size_policy_t key_size_policy : 1;
  size_policy_t value_size_policy : 1;
} hashtable_t;

static inline hash_t ht_h1(hash_t hash);
static inline ht_ctrl_t ht_h2(hash_t hash);

static inline b8 ht_ctrl_flag_is_present(ht_ctrl_t ctrl);

static void ht_resize_and_rehash(hashtable_t *table);

static inline void *ht_get_key_slot_addr(hashtable_t *table, sz pos);
static inline void *ht_get_value_slot_addr(hashtable_t *table, sz pos);

static void ht_set_key(hashtable_t *table, sz pos, const void *key);
static void ht_set_value(hashtable_t *table, sz pos, const void *value);

hashtable_t *__hashtable_create_impl(
  sz key_size, sz value_size, hash_function_t hash_function,
  size_policy_t key_size_policy, size_policy_t value_size_policy,
  get_size_function_t get_key_size_function,
  get_size_function_t get_value_size_function,
  compare_function_t key_compare_function)
{
  SOLC_ASSUME(hash_function != nullptr);

  hashtable_t *out_table = malloc(sizeof(hashtable_t));

  if (key_size_policy == SIZE_POLICY_VARIABLE) {
    key_size = sizeof(void *);
  }
  if (value_size_policy == SIZE_POLICY_VARIABLE) {
    value_size = sizeof(void *);
  }

  void *block = malloc(FULL_BLOCK_SIZE(key_size, value_size, HT_INITIAL_SIZE));

  out_table->ctrl = block;
  memset(out_table->ctrl, HT_CTRL_FLAG_EMPTY, CTRL_BLOCK_SIZE(HT_INITIAL_SIZE));

  out_table->slots = (char *)block + CTRL_BLOCK_SIZE(HT_INITIAL_SIZE);
  memset(out_table->slots, 0,
         KEY_BLOCK_SIZE(key_size, HT_INITIAL_SIZE) +
           VALUE_BLOCK_SIZE(value_size, HT_INITIAL_SIZE));

  out_table->size = HT_INITIAL_SIZE;
  out_table->filled = 0;
  out_table->key_size = key_size;
  out_table->value_size = value_size;
  out_table->hash_function = hash_function;
  out_table->get_key_size_function = get_key_size_function;
  out_table->get_value_size_function = get_value_size_function;
  out_table->key_compare_function = key_compare_function;
  out_table->key_size_policy = key_size_policy;
  out_table->value_size_policy = value_size_policy;

  return out_table;
}

void hashtable_destroy(hashtable_t *table)
{
  SOLC_ASSUME(table != nullptr);

  if (table->key_size_policy == SIZE_POLICY_VARIABLE ||
      table->value_size_policy == SIZE_POLICY_VARIABLE) {
    for (sz i = 0; i < table->size; i++) {
      if (ht_ctrl_flag_is_present(table->ctrl[i]))
        continue;

      if (table->key_size_policy == SIZE_POLICY_VARIABLE) {
        void *key = ht_get_key_slot_addr(table, i);
        key = *(void **)key;
        free(key);
      }

      if (table->value_size_policy == SIZE_POLICY_VARIABLE) {
        void *value = ht_get_value_slot_addr(table, i);
        value = *(void **)value;
        free(value);
      }
    }
  }

  free(table->ctrl);
  free(table);
}

void __hashtable_put_impl(hashtable_t *table, const void *key,
                          const void *value)
{
  SOLC_ASSUME(table != nullptr && key != nullptr && value != nullptr);

  hash_t hash = table->hash_function(key);
  sz pos = ht_h1(hash) % table->size;
  ht_ctrl_t h2 = ht_h2(hash);

  while (1) {
    if (h2 == table->ctrl[pos]) {
      void *slot_key = ht_get_key_slot_addr(table, pos);
      if (table->key_size_policy == SIZE_POLICY_VARIABLE) {
        if SOLC_UNLIKELY (table->key_compare_function == nullptr)
          SOLC_NOREACH();

        slot_key = *(void **)slot_key;
        SOLC_ASSUME(slot_key != nullptr);

        if SOLC_LIKELY (table->key_compare_function(key, slot_key)) {
          ht_set_value(table, pos, value);
          return;
        }
      } else if SOLC_LIKELY (memcmp(key, slot_key, table->key_size) == 0) {
        ht_set_value(table, pos, value);
        return;
      }
    }

    if (ht_ctrl_flag_is_present(table->ctrl[pos])) {
      ht_set_key(table, pos, key);
      ht_set_value(table, pos, value);

      table->ctrl[pos] = h2;
      table->filled++;

      float load_factor = (float)table->filled / table->size;
      if SOLC_UNLIKELY (load_factor > HT_MAX_LOAD_FACTOR) {
        ht_resize_and_rehash(table);
      }

      return;
    }

    pos = (pos + 1) % table->size;
  }
}

const void *__hashtable_get_impl(hashtable_t *table, const void *key)
{
  SOLC_ASSUME(table != nullptr && key != nullptr);

  hash_t hash = table->hash_function(key);
  sz pos = ht_h1(hash) % table->size;
  ht_ctrl_t h2 = ht_h2(hash);

  while (1) {
    if (h2 == table->ctrl[pos]) {
      void *slot_key = ht_get_key_slot_addr(table, pos);
      if (table->key_size_policy == SIZE_POLICY_VARIABLE) {
        if SOLC_UNLIKELY (table->key_compare_function == nullptr)
          SOLC_NOREACH();

        slot_key = *(void **)slot_key;
        SOLC_ASSUME(slot_key != nullptr);

        if SOLC_LIKELY (table->key_compare_function(key, slot_key)) {
          void *value = ht_get_value_slot_addr(table, pos);
          return table->value_size_policy == SIZE_POLICY_VARIABLE ?
                   *(void **)value :
                   value;
        }
      } else if SOLC_LIKELY (memcmp(key, slot_key, table->key_size) == 0) {
        void *value = ht_get_value_slot_addr(table, pos);
        return table->value_size_policy == SIZE_POLICY_VARIABLE ?
                 *(void **)value :
                 value;
      }
    }

    if SOLC_UNLIKELY (table->ctrl[pos] == HT_CTRL_FLAG_EMPTY)
      return nullptr;

    pos = (pos + 1) % table->size;
  }
}

void hashtable_remove(hashtable_t *table, const void *key)
{
  SOLC_ASSUME(table != nullptr && key != nullptr);

  hash_t hash = table->hash_function(key);
  sz pos = ht_h1(hash) % table->size;

  while (1) {
    if (ht_h2(hash) == table->ctrl[pos]) {
      void *slot_key = ht_get_key_slot_addr(table, pos);
      if (table->key_size_policy == SIZE_POLICY_VARIABLE) {
        if SOLC_UNLIKELY (table->key_compare_function == nullptr)
          SOLC_NOREACH();

        if SOLC_LIKELY (table->key_compare_function == nullptr) {
          void **key_slot_ptr = slot_key;
          void **value_slot_ptr = ht_get_value_slot_addr(table, pos);

          SOLC_ASSUME(value_slot_ptr != nullptr);

          free(*key_slot_ptr);
          *key_slot_ptr = 0;

          if (table->value_size_policy == SIZE_POLICY_VARIABLE) {
            free(*value_slot_ptr);
            *value_slot_ptr = 0;
          } else {
            memset(value_slot_ptr, 0, table->value_size);
          }

          table->ctrl[pos] = HT_CTRL_FLAG_DELETED;
          table->filled--;
          return;
        }
      } else if SOLC_LIKELY (memcmp(key, slot_key, table->key_size) == 0) {
        memset(slot_key, 0, table->key_size);

        void **value_slot_ptr = ht_get_value_slot_addr(table, pos);
        if (table->value_size_policy == SIZE_POLICY_VARIABLE) {
          free(*value_slot_ptr);
          *value_slot_ptr = 0;
        } else {
          memset(value_slot_ptr, 0, table->value_size);
        }

        table->ctrl[pos] = HT_CTRL_FLAG_DELETED;
        table->filled--;
        return;
      }
    }

    if (table->ctrl[pos] == HT_CTRL_FLAG_EMPTY)
      return;

    pos = (pos + 1) % table->size;
  }
}

b8 hashtable_is_empty(hashtable_t *table)
{
  SOLC_ASSUME(table != nullptr);
  return table->filled != 0;
}

sz hashtable_get_size(hashtable_t *table)
{
  SOLC_ASSUME(table != nullptr);
  return table->filled;
}

void hashtable_foreach(hashtable_t *table,
                       hashtable_foreach_function_t foreach_function)
{
  SOLC_ASSUME(table != nullptr && foreach_function != nullptr);
  for (sz i = 0; i < table->size; i++) {
    if (ht_ctrl_flag_is_present(table->ctrl[i]))
      continue;

    void *key_slot = ht_get_key_slot_addr(table, i);
    void *value_slot = ht_get_value_slot_addr(table, i);

    key_slot = table->key_size_policy == SIZE_POLICY_VARIABLE ?
                 *(void **)key_slot :
                 key_slot;
    value_slot = table->value_size_policy == SIZE_POLICY_VARIABLE ?
                   *(void **)value_slot :
                   value_slot;

    foreach_function(key_slot, value_slot);
  }
}

static inline sz ht_h1(sz hash)
{
  return hash >> 7;
}

static inline ht_ctrl_t ht_h2(sz hash)
{
  return hash & 0x7F;
}

static inline b8 ht_ctrl_flag_is_present(ht_ctrl_t ctrl)
{
  return (ctrl & 0b10000000) != 0;
}

static void ht_resize_and_rehash(hashtable_t *table)
{
  SOLC_ASSUME(table != nullptr);

  sz new_size = table->size * HT_RESIZE_FACTOR;

  sz new_memreq = FULL_BLOCK_SIZE(table->key_size, table->value_size, new_size);
  void *new_block = malloc(new_memreq);

  const sz ctrl_block_size = CTRL_BLOCK_SIZE(new_size);

  ht_ctrl_t *new_ctrl = new_block;
  memset(new_ctrl, HT_CTRL_FLAG_EMPTY, ctrl_block_size);

  void *new_slots = (char *)new_block + ctrl_block_size;
  memset(new_slots, 0,
         KEY_BLOCK_SIZE(table->key_size, new_size) +
           VALUE_BLOCK_SIZE(table->value_size, new_size));

  hashtable_t new_table;
  memcpy(&new_table, table, sizeof(hashtable_t));
  new_table.ctrl = new_ctrl;
  new_table.slots = new_slots;
  new_table.size = new_size;
  new_table.filled = 0;

  for (sz i = 0; i < table->size; i++) {
    if (ht_ctrl_flag_is_present(table->ctrl[i]))
      continue;

    void *key = ht_get_key_slot_addr(table, i);
    void *value = ht_get_value_slot_addr(table, i);

    if (table->key_size_policy == SIZE_POLICY_VARIABLE)
      key = *(void **)key;

    if (table->value_size_policy == SIZE_POLICY_VARIABLE)
      value = *(void **)value;

    __hashtable_put_impl(&new_table, key, value);
  }

  free(table->ctrl);
  memcpy(table, &new_table, sizeof(hashtable_t));
}

static inline void *ht_get_key_slot_addr(hashtable_t *table, sz pos)
{
  SOLC_ASSUME(table != nullptr);
  return (char *)table->slots + (table->key_size * pos);
}

static inline void *ht_get_value_slot_addr(hashtable_t *table, sz pos)
{
  SOLC_ASSUME(table != nullptr);
  return (char *)table->slots + (KEY_BLOCK_SIZE(table->key_size, table->size) +
                                 (table->value_size * pos));
}

static void ht_set_key(hashtable_t *table, sz pos, const void *key)
{
  SOLC_ASSUME(table != nullptr && key != nullptr);
  void *key_slot = ht_get_key_slot_addr(table, pos);

  if (table->key_size_policy == SIZE_POLICY_VARIABLE) {
    void **key_ptr = key_slot;
    if (*key_ptr != 0)
      free(*key_ptr);

    sz key_size = table->get_key_size_function(key);
    *key_ptr = malloc(key_size);
    memcpy(*key_ptr, key, key_size);

    return;
  }

  memcpy(key_slot, key, table->key_size);
}

static void ht_set_value(hashtable_t *table, sz pos, const void *value)
{
  SOLC_ASSUME(table != nullptr && value != nullptr);
  void *value_slot = ht_get_value_slot_addr(table, pos);

  if (table->value_size_policy == SIZE_POLICY_VARIABLE) {
    void **value_ptr = value_slot;
    if (*value_ptr != 0)
      free(*value_ptr);

    sz value_size = table->get_value_size_function(value);
    *value_ptr = malloc(value_size);
    memcpy(*value_ptr, value, value_size);

    return;
  }

  memcpy(value_slot, value, table->value_size);
}

b8 hashtable_key_compare_function_cstr(const void *key1, const void *key2)
{
  SOLC_ASSUME(key1 != nullptr && key2 != nullptr);
  return strcmp(key1, key2) == 0;
}

sz hashtable_get_size_function_cstr(const void *x)
{
  SOLC_ASSUME(x != nullptr);
  return strlen(x) + 1;
}

hash_t hash_function_UNDEFINED(const void *x)
{
  SOLC_NOREACH();
}
