#include "util.hpp"
#include <string>

static const char *prefix_ = "\033[";
static const char *suffix_ = "m";

static const char *colors_[] = {
  "30", "31", "32", "33", "34", "35", "36", "37", "39",
};

static const char *gmodes_[] = {
  "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
};

std::string escape_reset()
{
  return "\033[0m";
}

std::string escape_color(ESCColor col)
{
  std::string out{};
  out += prefix_;
  out += colors_[col];
  out += suffix_;
  return out;
}

std::string escape_graphics(ESCGraphics mode)
{
  std::string out{};
  out += prefix_;
  out += gmodes_[mode];
  out += suffix_;
  return out;
}
