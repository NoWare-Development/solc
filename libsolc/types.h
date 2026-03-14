#ifndef __SOLC_TYPES_H__
#define __SOLC_TYPES_H__

#include "solc/defs.h"

typedef b8 (*compare_function_t)(const void *a, const void *b);
typedef sz (*get_size_function_t)(const void *x);

typedef enum {
  SIZE_POLICY_FIXED = 0,
  SIZE_POLICY_VARIABLE = 1,
} size_policy_t;

b8 key_compare_function_cstr(const void *key1, const void *key2);

sz get_size_function_cstr(const void *x);

#define get_default_get_size_function(_X) \
  _Generic((_X),                          \
    char *: get_size_function_cstr,       \
    const char *: get_size_function_cstr, \
    default: nullptr)

#define get_default_key_compare_function(_key) \
  _Generic((_key),                             \
    char *: key_compare_function_cstr,         \
    const char *: key_compare_function_cstr,   \
    default: nullptr)

#define get_default_size_policy(_X)     \
  _Generic((_X),                        \
    char *: SIZE_POLICY_VARIABLE,       \
    const char *: SIZE_POLICY_VARIABLE, \
    default: SIZE_POLICY_FIXED)

#endif // __SOLC_TYPES_H__
