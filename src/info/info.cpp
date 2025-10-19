#include "info.hpp"
#include <iostream>

namespace info
{

// TODO: move me into something like `solcinfo.hpp.in` and preprocess it with
// meson.
#define NLC_VERSION "1.0.0"
#define COPYRIGHT_YEAR "2025"

static const char *help_message =
  "Usage: solc [options] file...\n"
  "Options:\n"
  "  -help             Display this information\n"
  "  -version          Display version\n"
  "  -opt=<level>      Set optimization level\n"
  "  -include=[paths]  Add include paths\n"
  "  -libpath=[paths]  Add library search paths\n"
  "  -linkwith=[libs]  Add libraries to link with\n"
  "  -nostdlib         Disable standard library\n"
  "  -freestanding     Disable runtime\n"

  ;

static const char *version_message = "solc (NLC) " NLC_VERSION "\n"
                                     "Copyright (C) " COPYRIGHT_YEAR
                                     " NoWare-Development\n";

void display_help()
{
  std::cout << help_message << '\n';
}

void display_version()
{
  std::cout << version_message << '\n';
}

}
