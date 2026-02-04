#ifndef __SOLC_CONTAINER_HASHTABLE_H__
#define __SOLC_CONTAINER_HASHTABLE_H__

#include "solc/defs.h"

#include "hash.h"
#include "types.h"

typedef struct __hashtable_t hashtable_t;

typedef void (*hashtable_foreach_function_t)(const void *key,
                                             const void *value);

#define hashtable_create(_table_ptr, _key_type, _value_type)        \
  {                                                                 \
    _key_type __hashtable_phantom_key__;                            \
    _value_type __hashtable_phantom_value__;                        \
    (_table_ptr) = __hashtable_create_impl(                         \
      sizeof(_key_type), sizeof(_value_type),                       \
      get_default_hash_function(__hashtable_phantom_key__),         \
      get_default_size_policy(__hashtable_phantom_key__),           \
      get_default_size_policy(__hashtable_phantom_value__),         \
      get_default_get_size_function(__hashtable_phantom_key__),     \
      get_default_get_size_function(__hashtable_phantom_value__),   \
      get_default_key_compare_function(__hashtable_phantom_key__)); \
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

#define hashtable_remove(_table, _key) \
  __hashtable_remove_impl((_table), __hashtable_arg_macro((_key)))

#define hashtable_remove_raw(_table, _key) \
  __hashtable_remove_impl((_table), (_key))

void hashtable_destroy(hashtable_t *table);
b8 hashtable_is_empty(hashtable_t *table);
sz hashtable_get_size(hashtable_t *table);
void hashtable_foreach(hashtable_t *table,
                       hashtable_foreach_function_t foreach_function);

hashtable_t *__hashtable_create_impl(
  sz key_size, sz value_size, hash_function_t hash_function,
  size_policy_t key_size_policy, size_policy_t value_size_policy,
  get_size_function_t get_key_size_function,
  get_size_function_t get_value_size_function,
  compare_function_t key_compare_function);
void __hashtable_put_impl(hashtable_t *table, const void *key,
                          const void *value);
const void *__hashtable_get_impl(hashtable_t *table, const void *key);
void __hashtable_remove_impl(hashtable_t *table, const void *key);

#define __hashtable_arg_macro(_X) \
  _Generic((_X), char *: (_X), const char *: (_X), default: &(_X))

#endif // __SOLC_CONTAINER_HASHTABLE_H__
