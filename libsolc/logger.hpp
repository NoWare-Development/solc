#pragma once

// NOTE: this logger is for internal purposes only and works only in debug
// builds

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace solc
{

template <typename T>
static std::vector<std::string> __solc_print_format_args_(T &x)
{
  std::stringstream ss;
  ss << x;
  return (std::vector<std::string>){ ss.str() };
}

template <typename T, typename... Args>
static std::vector<std::string> __solc_print_format_args_(T &x, Args &&...args)
{
  auto a = __solc_print_format_args_(x);
  auto b = __solc_print_format_args_(args...);
  a.insert(a.end(), b.begin(), b.end());
  return a;
}

template <typename... Args>
static std::string __solc_print_format_(const std::string &fmt, Args &&...args)
{
  std::vector<std::string> argument_strings =
    __solc_print_format_args_(args...);
  auto arg_len = argument_strings.size();
  decltype(arg_len) arg_index = 0;

  auto fmt_len = fmt.length();

  std::string buf{};
  buf.reserve(fmt_len);

  for (std::string::size_type i = 0; i < fmt_len; i++) {
    char c = fmt.at(i);
    if (c == '{') {
      if (i + 1 < fmt_len) {
        char next_c = fmt.at(i + 1);
        if (next_c == '}') {
          if (arg_index >= argument_strings.size()) {
            throw std::runtime_error("Argument number (" +
                                     std::to_string(arg_len) +
                                     ") is less than required");
          }

          buf += argument_strings.at(arg_index++);
          i++;
          continue;
        } else if (next_c == '{') {
          buf += '{';
          i++;
          continue;
        }
      }
      throw std::runtime_error("Unterminated argument placeholder");
    }
    buf += c;
  }

  if (arg_index != arg_len) {
    throw std::runtime_error("Argument number (" + std::to_string(arg_len) +
                             ") is bigger than required");
  }

  return buf;
}

static inline std::string __solc_print_format_(const std::string &fmt)
{
  auto fmt_len = fmt.length();

  std::string buf{};
  buf.reserve(fmt_len);

  for (std::string::size_type i = 0; i < fmt_len; i++) {
    char c = fmt.at(i);
    if (c == '{') {
      if (i + 1 < fmt_len) {
        char next_c = fmt.at(i + 1);
        if (next_c == '}') {
          throw std::runtime_error("No arguments were provided");
        } else if (next_c == '{') {
          buf += '{';
          i++;
          continue;
        }
      }
      throw std::runtime_error("Unterminated argument placeholder");
    }
    buf += c;
  }

  return buf;
}

template <typename... Args>
static inline void print(const std::string &fmt, Args &&...args)
{
  std::cout << __solc_print_format_(fmt, args...);
}

static inline void print(const std::string &fmt)
{
  std::cout << __solc_print_format_(fmt);
}

template <typename... Args>
static inline void println(const std::string &fmt, Args &&...args)
{
  std::cout << __solc_print_format_(fmt, args...) << '\n';
}

static inline void println(const std::string &fmt)
{
  std::cout << __solc_print_format_(fmt) << '\n';
}

#ifdef _DEBUG
#define dbglog(fmt, ...)                                  \
  {                                                       \
    solc::println("[{}]: " fmt, __func__, ##__VA_ARGS__); \
  }
#else
#define dbglog(fmt, ...)
#endif

}
