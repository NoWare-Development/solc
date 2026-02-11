#ifndef __SOLC_CONTAINER_STRING_H__
#define __SOLC_CONTAINER_STRING_H__

#include "solc/defs.h"

typedef struct {
  sz size;
  char *data;
} string_t;

string_t string_create(void);
string_t string_create_from(const char *c_str);
string_t string_copy(const string_t *str);
void string_destroy(string_t *str);
char string_at(string_t *str, sz pos);
void string_append(string_t *dst, string_t *src);
sz string_length(const string_t *str);

#endif // __SOLC_CONTAINER_STRING_H__
