#ifndef __SOLC_CONTAINER_HASHTABLE_H__
#define __SOLC_CONTAINER_HASHTABLE_H__

#include "solc/defs.h"

#include "hash.h"

typedef struct __hashtable_t hashtable_t;

typedef b8 (*hashtable_key_compare_function_t)(const void *key1,
                                               const void *key2);
typedef sz (*hashtable_get_size_function_t)(const void *x);
typedef void (*hashtable_foreach_function_t)(const void *key,
                                             const void *value);

// * HASHTABLE_SIZE_POLICY_FIXED stores data of an object provided
//   to the `hashtable_put()'.
// * HASHTABLE_SIZE_POLICY_VARIABLE stores a pointer to a copy of data
//   of an object provided to the `hashtable_put()' (function pointer of type
//   `hashtable_get_size_function_t' must be provided in `hashtable_create()').
typedef enum {
  HASHTABLE_SIZE_POLICY_FIXED = 0,
  HASHTABLE_SIZE_POLICY_VARIABLE = 1,
} hashtable_size_policy_t;

#define hashtable_create(_table_ptr, _key_type, _value_type)                  \
  {                                                                           \
    _key_type __hashtable_phantom_key__;                                      \
    _value_type __hashtable_phantom_value__;                                  \
    (_table_ptr) = __hashtable_create_impl(                                   \
      sizeof(_key_type), sizeof(_value_type),                                 \
      __hashtable_get_default_hash_function(__hashtable_phantom_key__),       \
      __hashtable_get_default_size_policy(__hashtable_phantom_key__),         \
      __hashtable_get_default_size_policy(__hashtable_phantom_value__),       \
      __hashtable_get_default_get_size_function(__hashtable_phantom_key__),   \
      __hashtable_get_default_get_size_function(__hashtable_phantom_value__), \
      __hashtable_get_default_key_compare_function(                           \
        __hashtable_phantom_key__));                                          \
  }

#define hashtable_create_raw(_key_type, _value_type, _hash_function_ptr, \
                             _key_size_policy, _value_size_policy,       \
                             _get_key_size_function_ptr,                 \
                             _get_value_size_function_ptr,               \
                             _key_compare_function_ptr)                  \
  hashtable_create(sizeof(_key_type), sizeof(_value_type),               \
                   (_hash_function_ptr), (_key_size_policy),             \
                   (_value_size_policy), (_get_key_size_function_ptr),   \
                   (_get_value_size_function_ptr),                       \
                   (_key_compare_function_ptr))

#define hashtable_put(_table, _key, _value)                     \
  __hashtable_put_impl((_table), __hashtable_arg_macro((_key)), \
                       __hashtable_arg_macro((_value)))

#define hashtable_put_raw(_table, _key, _value) \
  __hashtable_put_impl((_table), (_key), (_value))

#define hashtable_get(_table, _key) \
  __hashtable_get_impl((_table), __hashtable_arg_macro((_key)))

#define hashtable_get_raw(_table, _key) __hashtable_get_impl((_table), (_key))

void hashtable_destroy(hashtable_t *table);
void hashtable_remove(hashtable_t *table, const void *key);
b8 hashtable_is_empty(hashtable_t *table);
sz hashtable_get_size(hashtable_t *table);
void hashtable_foreach(hashtable_t *table,
                       hashtable_foreach_function_t foreach_function);

b8 hashtable_key_compare_function_cstr(const void *key1, const void *key2);
sz hashtable_get_size_function_cstr(const void *x);

hash_t hash_function_UNDEFINED(const void *x);

hashtable_t *
__hashtable_create_impl(sz key_size, sz value_size,
                        hash_function_t hash_function,
                        hashtable_size_policy_t key_size_policy,
                        hashtable_size_policy_t value_size_policy,
                        hashtable_get_size_function_t get_key_size_function,
                        hashtable_get_size_function_t get_value_size_function,
                        hashtable_key_compare_function_t key_compare_function);
void __hashtable_put_impl(hashtable_t *table, const void *key,
                          const void *value);
const void *__hashtable_get_impl(hashtable_t *table, const void *key);

#define __hashtable_get_default_get_size_function(_X) \
  _Generic((_X),                                      \
    char *: hashtable_get_size_function_cstr,         \
    const char *: hashtable_get_size_function_cstr,   \
    default: nullptr)

#define __hashtable_get_default_key_compare_function(_key) \
  _Generic((_key),                                         \
    char *: hashtable_key_compare_function_cstr,           \
    const char *: hashtable_key_compare_function_cstr,     \
    default: nullptr)

#define __hashtable_get_default_hash_function(_key) \
  _Generic((_key),                                  \
    s8: hash_function_i8,                           \
    s16: hash_function_i16,                         \
    s32: hash_function_i32,                         \
    s64: hash_function_i64,                         \
    u8: hash_function_i8,                           \
    u16: hash_function_i16,                         \
    u32: hash_function_i32,                         \
    u64: hash_function_i64,                         \
    char *: hash_function_fnv_1a_cstr,              \
    const char *: hash_function_fnv_1a_cstr,        \
    default: hash_function_UNDEFINED)

#define __hashtable_get_default_size_policy(_X)   \
  _Generic((_X),                                  \
    char *: HASHTABLE_SIZE_POLICY_VARIABLE,       \
    const char *: HASHTABLE_SIZE_POLICY_VARIABLE, \
    default: HASHTABLE_SIZE_POLICY_FIXED)

#define __hashtable_arg_macro(_X) \
  _Generic((_X), char *: (_X), const char *: (_X), default: &(_X))

#endif // __SOLC_CONTAINER_HASHTABLE_H__
