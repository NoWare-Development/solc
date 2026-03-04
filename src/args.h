#ifndef __SOLC_ARGS_H__
#define __SOLC_ARGS_H__

#include <solc/defs.h>

void args_set(s32 argc, char **argv);
b8 arg_present(const char *full, const char *small);
b8 arg_get_value(const char *full, const char *small, char out[static 1024],
                 sz n);
s32 arg_get_dangling(s32 *out_indices);

#endif // __SOLC_ARGS_H__
