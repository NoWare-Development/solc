#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>

#define TODO(msg)                                                             \
  {                                                                           \
    std::printf ("(%s:%d) Not implemented yet: %s\n", __func__, __LINE__,     \
                 msg);                                                        \
    exit (-1);                                                                \
  }

#define NEVERREACH()                                                          \
  {                                                                           \
    std::printf ("(%s:%d) Reached point that should be never reached.\n",     \
                 __func__, __LINE__);                                         \
    exit (-1);                                                                \
  }

namespace nlc::util
{

std::string string_lowercase (const std::string &str);

}
