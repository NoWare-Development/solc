#include "util.hpp"
#include <algorithm>
#include <cctype>
#include <string>

namespace solc::util
{

std::string
string_lowercase (const std::string &str)
{
  std::string out = str;

  std::transform (out.begin (), out.end (), out.begin (),
                  [] (unsigned char c) { return std::tolower (c); });

  return out;
}

}
