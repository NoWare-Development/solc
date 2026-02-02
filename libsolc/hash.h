#ifndef __SOLC_HASH_H__
#define __SOLC_HASH_H__

#include "solc/defs.h"

typedef sz hash_t;

typedef hash_t (*hash_function_t)(const void *key);

hash_t hash_function_fnv_1a_cstr(const void *key);
hash_t hash_function_i8(const void *key);
hash_t hash_function_i16(const void *key);
hash_t hash_function_i32(const void *key);
hash_t hash_function_i64(const void *key);

#endif // __SOLC_HASH_H__
