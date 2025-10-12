#pragma once
#include <iostream>

#define MOVE(obj) std::move (obj)

#define VERIFY_POS(pos)                                                       \
  {                                                                           \
    if (!verify_pos ((pos)))                                                  \
      {                                                                       \
        std::cerr << "ERROR <EXPECTED(token)> IN FUNCTION " << __FUNCTION__   \
                  << " " << __LINE__ << '\n';                                 \
        return {};                                                            \
      }                                                                       \
  }

#define VERIFY_TOKEN(pos, got, expected)                                      \
  {                                                                           \
    if (!verify_token ((pos), (got), (expected)))                             \
      {                                                                       \
        std::cerr << "ERROR <UNEXPECTED(token)> IN FUNCTION " << __FUNCTION__ \
                  << " " << __LINE__ << '\n';                                 \
        return {};                                                            \
      }                                                                       \
  }

#define VERIFY_VALUE(pos, got, expected)                                      \
  {                                                                           \
    if (!verify_value ((pos), (got), (expected)))                             \
      {                                                                       \
        std::cerr << "ERROR <UNEXPECTED(value)> IN FUNCTION " << __FUNCTION__ \
                  << " " << __LINE__ << '\n';                                 \
        return {};                                                            \
      }                                                                       \
  }
