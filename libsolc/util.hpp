#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include "logger.hpp"

#define TODO(msg)                                                              \
  {                                                                            \
    solc::println("({}:{}) Not implemented yet: {}", __func__, __LINE__, msg); \
    exit(-1);                                                                  \
  }

#define NOREACH()                                                        \
  {                                                                      \
    solc::println("({}:{}) Reached point that should be never reached.", \
                  __func__, __LINE__);                                   \
    exit(-1);                                                            \
  }

#define ERROR(msg)                                        \
  {                                                       \
    solc::println("({}:{}) {}", __func__, __LINE__, msg); \
    exit(-1);                                             \
  }

namespace solc::util
{

std::string string_lowercase(const std::string &str);
size_t string_to_int(const std::string &str);
double string_to_double(const std::string &str);

}
