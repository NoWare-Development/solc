#ifndef __SOLC_TYPES_H__
#define __SOLC_TYPES_H__

#include "solc/defs.h"

typedef b8 (*compare_function_t)(const void *a, const void *b);
typedef sz (*get_size_function_t)(const void *x);

typedef enum {
  SIZE_POLICY_FIXED = 0,
  SIZE_POLICY_VARIABLE = 1,
} size_policy_t;

#endif // __SOLC_TYPES_H__
