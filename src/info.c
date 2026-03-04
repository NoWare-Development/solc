#include "info.h"
#include <solc/defs.h>

#define SOLC_VERSION 0.0.1
#define SOLC_COPYRIGHT_YEAR 2026
#define SOLC_COMPILER_DEVELOPER "NoWare-Development"

static const char *_help_message = "Usage: solc [options] file...\n"
                                   "Options:\n"
                                   "  --help -h     Display this information\n"
                                   "  --version -v  Display version";

static const char *_version_message = "solc (SOLC) " __SOLC_MACRO_STR(
  SOLC_VERSION) "\n"
                "Copyright (C) " __SOLC_MACRO_STR(
                  SOLC_COPYRIGHT_YEAR) " " SOLC_COMPILER_DEVELOPER "\n"
                                       "This is free software; see the source for copying conditions (not ready yet).  There is NO\n"
                                       "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.";

void display_help(void)
{
  puts(_help_message);
}

void display_version(void)
{
  puts(_version_message);
}
