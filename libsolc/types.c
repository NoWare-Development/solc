#include "types.h"
#include <string.h>

b8 key_compare_function_cstr(const void *key1, const void *key2)
{
  SOLC_ASSUME(key1 != nullptr && key2 != nullptr);
  return strcmp(key1, key2) == 0;
}

sz get_size_function_cstr(const void *x)
{
  SOLC_ASSUME(x != nullptr);
  return strlen(x) + 1;
}
