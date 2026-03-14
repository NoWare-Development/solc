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

#define get_default_hash_function(_key)      \
  _Generic((_key),                           \
    s8: hash_function_i8,                    \
    s16: hash_function_i16,                  \
    s32: hash_function_i32,                  \
    s64: hash_function_i64,                  \
    u8: hash_function_i8,                    \
    u16: hash_function_i16,                  \
    u32: hash_function_i32,                  \
    u64: hash_function_i64,                  \
    char *: hash_function_fnv_1a_cstr,       \
    const char *: hash_function_fnv_1a_cstr, \
    default: hash_function_UNDEFINED)

hash_t hash_function_UNDEFINED(const void *x);

#endif // __SOLC_HASH_H__
