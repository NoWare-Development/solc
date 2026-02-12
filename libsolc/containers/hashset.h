#ifndef __SOLC_CONTAINERS_HASHSET_H__
#define __SOLC_CONTAINERS_HASHSET_H__

#include "hash.h"
#include "solc/defs.h"
#include "types.h"

typedef struct __hashset_t hashset_t;

#define hashset_create(_set_ptr, _key_type)                                  \
  {                                                                          \
    _key_type __hashset_phantom_key__;                                       \
    (_set_ptr) = __hashset_create_impl(                                      \
      sizeof(_key_type), get_default_hash_function(__hashset_phantom_key__), \
      get_default_size_policy(__hashset_phantom_key__),                      \
      get_default_get_size_function(__hashset_phantom_key__),                \
      get_default_key_compare_function(__hashset_phantom_key__));            \
  }

#define hashset_create_raw(_key_type, _hash_function, _key_size_policy,   \
                           _get_key_size_function, _key_compare_function) \
  __hashset_create_impl(sizeof(_key_type), (_hash_function),              \
                        (_key_size_policy), (_get_key_size_function),     \
                        (_key_compare_function))

#define hashset_set(_set, _key)                                       \
  {                                                                   \
    (_set) = __hashset_set_impl((_set), __hashset_arg_macro((_key))); \
  }
#define hashset_set_raw(_set, _key)              \
  {                                              \
    (_set) = __hashset_set_impl((_set), (_key)); \
  }

#define hashset_is_set(_set, _key) \
  __hashset_is_set_impl((_set), __hashset_arg_macro((_key)))
#define hashset_is_set_raw(_set, _key) __hashset_is_set_impl((_set), (_key))

#define hashset_unset(_set, _key) \
  __hashset_unset_impl((_set), __hashset_arg_macro((_key)))
#define hashset_unset_raw(_set, _key) __hashset_unset_impl((_set), (_key))

hashset_t *__hashset_create_impl(sz key_size, hash_function_t hash_function,
                                 size_policy_t key_size_policy,
                                 get_size_function_t get_key_size_function,
                                 compare_function_t key_compare_function);
void hashset_destroy(hashset_t *set);
hashset_t *__hashset_set_impl(hashset_t *set, const void *key);
b8 __hashset_is_set_impl(hashset_t *set, const void *key);
void __hashset_unset_impl(hashset_t *set, const void *key);

sz hashset_get_size(hashset_t *set);
b8 hashset_is_empty(hashset_t *set);

#define __hashset_arg_macro(_X) \
  _Generic((_X), char *: (_X), const char *: (_X), default: &(_X))

#endif // __SOLC_CONTAINERS_HASHSET_H__
