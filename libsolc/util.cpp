#include "util.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>

namespace solc::util
{

std::string string_lowercase(const std::string &str)
{
  std::string out = str;

  std::transform(out.begin(), out.end(), out.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  return out;
}

size_t string_to_int(const std::string &str)
{
  size_t out{};
  for (char c : str) {
    if (c >= '0' && c <= '9') {
      out = (out << 4) | (c - '0');
    } else if (c >= 'a' && c <= 'f') {
      out = (out << 4) | (c - 'a' + 0xA);
    } else if (c >= 'A' && c <= 'F') {
      out = (out << 4) | (c - 'A' + 0xA);
    }
  }
  return out;
}

double string_to_double(const std::string &str)
{
  return atof(str.c_str());
}

}
