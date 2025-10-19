#pragma once

#ifdef _DEBUG
#include "libsolc/logger.hpp"
#endif

#define MOVE(obj) std::move (obj)

#ifdef _DEBUG
#define VERIFY_POS(pos)                                                       \
  {                                                                           \
    if (!verify_pos ((pos)))                                                  \
      {                                                                       \
        info ("ERROR <EXPECTED(token)> " << __LINE__);                        \
        return {};                                                            \
      }                                                                       \
  }

#define VERIFY_TOKEN(pos, got, expected)                                      \
  {                                                                           \
    if (!verify_token ((pos), (got), (expected)))                             \
      {                                                                       \
        info ("ERROR <UNEXPECTED(token)> " << __LINE__);                      \
        return {};                                                            \
      }                                                                       \
  }

#define VERIFY_VALUE(pos, got, expected)                                      \
  {                                                                           \
    if (!verify_value ((pos), (got), (expected)))                             \
      {                                                                       \
        info ("ERROR <UNEXPECTED(value)> " << __LINE__);                      \
        return {};                                                            \
      }                                                                       \
  }
#else
#define VERIFY_POS(pos)                                                       \
  {                                                                           \
    if (!verify_pos ((pos)))                                                  \
      return {};                                                              \
  }

#define VERIFY_TOKEN(pos, got, expected)                                      \
  {                                                                           \
    if (!verify_token ((pos), (got), (expected)))                             \
      return {};                                                              \
  }

#define VERIFY_VALUE(pos, got, expected)                                      \
  {                                                                           \
    if (!verify_value ((pos), (got), (expected)))                             \
      return {};                                                              \
  }
#endif
