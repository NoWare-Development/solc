#pragma once

// NOTE: this logger is for internal purposes only and works only in debug
// builds

#include <iostream>
#include <sstream>
#include <string>

namespace solc
{

#ifdef _DEBUG
class Logger
{
public:
  Logger () = default;
  ~Logger () { std::cout << buf << '\n'; }

  template <typename T>
  Logger &
  operator<< (const T &x)
  {
    std::stringstream ss;
    ss << x;
    buf += ss.str ();
    return *this;
  }

private:
  std::string buf{};
};

#define info(...)                                                             \
  {                                                                           \
    solc::Logger __solc_logger_##__COUNTER__##__{};                           \
    __solc_logger_##__COUNTER__##__ << '[' << __func__                        \
                                    << "]: " << __VA_ARGS__;                  \
  }
}
#else
#define info(...)
#endif
