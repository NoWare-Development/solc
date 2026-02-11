#include "containers/string.h"
#include "solc/defs.h"
#include <stdlib.h>
#include <string.h>

string_t string_create(void)
{
  char *data = malloc(sizeof(char) * 1);
  data[0] = 0;
  string_t out = {
    .size = 1,
    .data = data,
  };
  return out;
}

string_t string_create_from(const char *c_str)
{
  SOLC_ASSUME(c_str != nullptr);
  const sz c_str_size = strlen(c_str) + 1;
  char *data = malloc(sizeof(char) * c_str_size);
  memcpy(data, c_str, sizeof(char) * c_str_size);
  string_t out = {
    .size = c_str_size,
    .data = data,
  };
  return out;
}

string_t string_copy(const string_t *str)
{
  SOLC_ASSUME(str != nullptr && str->data != nullptr && str->size > 0);
  char *data = malloc(sizeof(char) * str->size);
  memcpy(data, str->data, sizeof(char) * str->size);
  string_t out = {
    .size = str->size,
    .data = data,
  };
  return out;
}

void string_destroy(string_t *str)
{
  SOLC_ASSUME(str != nullptr && str->data != nullptr);
  free(str->data);
  str->size = 0;
  str->data = nullptr;
}

char string_at(string_t *str, sz pos)
{
  return pos < str->size ? str->data[pos] : 0;
}

void string_append(string_t *dst, string_t *src)
{
  char *new_data = malloc(sizeof(char) * (dst->size - 1 + src->size));
  memcpy(new_data, dst->data, sizeof(char) * (dst->size - 1));
  memcpy(new_data + (dst->size - 1), src->data, src->size);
  free(dst->data);
  dst->data = new_data;
  dst->size += src->size - 1;
}

sz string_length(const string_t *str)
{
  SOLC_ASSUME(str != nullptr && str->size > 0);
  return str->size - 1;
}
