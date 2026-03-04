#include "args.h"
#include <solc/defs.h>
#include <stdlib.h>
#include <string.h>

static s32 _argc;
static char **_argv;

void args_set(s32 argc, char **argv)
{
  _argc = argc;
  _argv = argv;
}

b8 arg_present(const char *full, const char *small)
{
  SOLC_ASSUME(full != nullptr || small != nullptr);

  for (s32 i = 1; i < _argc; i++) {
    if ((full != nullptr && strcmp(full, _argv[i]) == 0) ||
        (small != nullptr && strcmp(small, _argv[i]) == 0))
      return true;
  }

  return false;
}

b8 arg_get_value(const char *full, const char *small, char out[static 1024],
                 sz n)
{
  SOLC_ASSUME(full != nullptr || small != nullptr);

  memset(out, 0, sizeof(char) * 1024);

  char *key = calloc(1024, sizeof(char));
  char *value = calloc(1024, sizeof(char));

  b8 result = false;

  for (s32 i = 1; i < _argc; i++) {
    s32 eqpos = -1;

    for (char *arg = _argv[i], *p_key = key, *p_value = value; *arg; arg++) {
      if (eqpos == -1) {
        if (*arg == '=') {
          eqpos = arg - _argv[i];
          continue;
        }

        *p_key++ = *arg;
        continue;
      }

      *p_value++ = *arg;
    }

    if ((full != nullptr && strcmp(key, full) == 0) ||
        (small != nullptr && strcmp(key, small) == 0)) {
      memcpy(out, value, n - 1);
      out[n - 1] = 0;
      result = true;
      break;
    }

    memset(key, 0, sizeof(char) * 1024);
    memset(value, 0, sizeof(char) * 1024);
  }

  free(key);
  free(value);

  return result;
}

s32 arg_get_dangling(s32 *out_indices)
{
  s32 num_dangling = 0;
  for (s32 i = 1; i < _argc; i++) {
    if (_argv[i] == nullptr || _argv[i][0] == '-')
      continue;

    if (out_indices != nullptr)
      out_indices[num_dangling] = i;
    num_dangling++;
  }

  return num_dangling;
}
