#ifndef __SOLC_CONTAINERS_HASHSET_H__
#define __SOLC_CONTAINERS_HASHSET_H__

#include "hash.h"
#include "solc/defs.h"
#include "types.h"

typedef struct __hashset_t hashset_t;

hashset_t *__hashset_create_impl(sz key_size, hash_function_t hash_function,
                                 size_policy_t key_size_policy,
                                 get_size_function_t get_key_size_function,
                                 compare_function_t key_compare_function);
void hashset_destroy(hashset_t *set);
void __hashset_set_impl(hashset_t *set, const void *key);
b8 __hashset_is_set(hashset_t *set, const void *key);
void __hashset_unset_impl(hashset_t *set, const void *key);

sz hashset_get_size(hashset_t *set);
b8 hashset_is_empty(hashset_t *set);

#endif // __SOLC_CONTAINERS_HASHSET_H__
