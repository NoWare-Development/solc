#ifndef __SOLC_ARGS_H__
#define __SOLC_ARGS_H__

#include <solc/defs.h>
#include <stdio.h>
#include <string.h>

typedef struct {
#define BOOLEAN_ARG(_name, _full, _small, _desc) b8 _name;
#define PREFIX_ARG(_name, _prefix, _desc, _val) \
  char _name[1024][1024];                       \
  s32 num_##_name;
#define VALUE_ARG(_name, _full, _small, _desc, _val) char _name[1024];
#ifdef ARGUMENTS
  ARGUMENTS
#endif
#undef BOOLEAN_ARG
#undef PREFIX_ARG
#undef VALUE_ARG

  s32 danlings[1024];
  s32 num_dangling;
} args_t;

static inline b8 args_parse(args_t *args, s32 argc, char **argv)
{
  SOLC_ASSUME(args != nullptr);

  b8 result = true;

  for (s32 i = 1; i < argc; i++) {
#define BOOLEAN_ARG(_name, _full, _small, _desc)                   \
  {                                                                \
    if (((_full) != nullptr && strcmp((_full), argv[i]) == 0) ||   \
        ((_small) != nullptr && strcmp((_small), argv[i]) == 0)) { \
      args->_name = true;                                          \
      continue;                                                    \
    }                                                              \
  }

#define PREFIX_ARG(_name, _prefix, _desc, _val)                              \
  {                                                                          \
    int result_##_name = 1;                                                  \
    char *arg_p = argv[i];                                                   \
    for (char *tmp_prefix = (_prefix); *tmp_prefix; tmp_prefix++, arg_p++) { \
      if (!*arg_p || *tmp_prefix != *arg_p)                                  \
        result_##_name = 0;                                                  \
    }                                                                        \
                                                                             \
    if (result_##_name) {                                                    \
      if (*arg_p) {                                                          \
        strncpy(args->_name[args->num_##_name++], arg_p, 1023);              \
      } else {                                                               \
        fprintf(stderr, "Expected " _val " after \"" _prefix "\".\n");       \
        result = false;                                                      \
      }                                                                      \
      continue;                                                              \
    }                                                                        \
  }

#define VALUE_ARG(_name, _full, _small, _desc, _val)               \
  {                                                                \
    if (((_full) != nullptr && strcmp((_full), argv[i]) == 0) ||   \
        ((_small) != nullptr && strcmp((_small), argv[i]) == 0)) { \
      if (i + 1 >= argc)                                           \
        return false;                                              \
      strncpy(args->_name, argv[++i], 1023);                       \
      continue;                                                    \
    }                                                              \
  }

#ifdef ARGUMENTS
    ARGUMENTS
#endif

#undef BOOLEAN_ARG
#undef PREFIX_ARG
#undef VALUE_ARG

    if (argv[i][0] == '-') {
      fprintf(stderr, "Unknown argument \"%s\".\n", argv[i]);
      result = false;
      continue;
    }

    args->danlings[args->num_dangling++] = i;
  }

  return result;
}

static inline void args_help(const char *top_msg)
{
  puts(top_msg);
  puts("Options:");

#define BOOLEAN_ARG(_name, _full, _small, _desc) \
  printf("  %-20s  " _desc "\n", _small " " _full);

#define PREFIX_ARG(_name, _prefix, _desc, _val) \
  printf("  %-20s  " _desc "\n", _prefix "<" _val ">");

#define VALUE_ARG(_name, _full, _small, _desc, _val) \
  printf("  %-20s  " _desc "\n", _small " " _full " <" _val ">");

#ifdef ARGUMENTS
  ARGUMENTS
#endif

#undef BOOLEAN_ARG
#undef PREFIX_ARG
#undef VALUE_ARG
}

#endif // __SOLC_ARGS_H__
