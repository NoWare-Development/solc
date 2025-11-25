#pragma once

#include "libsolc/logger.hpp"
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

class ArgParser {
  private:
  struct Argument {
    std::string name;
    std::string value;
  };

  public:
  ArgParser(int argc, char **argv)
  {
    for (int i = 1; i < argc; i++) {
      if (std::string(argv[i]).length() == 0)
        continue;

      if (is_dangling(argv[i])) {
        _danling_args.emplace_back(argv[i]);
        continue;
      }

      Argument arg = get_argument(argv[i]);
      if (has_argument(arg.name)) {
        std::cout << "Duplicate argument \"" << argv[i] << "\"\n";
      }

      _args[arg.name] = arg.value;
    }
  }

  ~ArgParser() = default;

  bool has_argument(const std::string &name) const
  {
    return _args.find(name) != _args.end();
  }

  const std::vector<std::string> &get_dangling_arguments() const
  {
    return _danling_args;
  }

  template <typename T> T get_argument_value(const std::string &name) const
  {
    if (!has_argument(name))
      return {};

    const auto &value = _args.at(name);
    if (value.length() == 0)
      return {};

    return string_to_value<T>(value);
  }

  template <typename T>
  std::vector<T> get_argument_value_list(const std::string &name) const
  {
    if (!has_argument(name))
      return {};

    auto values = _args.at(name);
    if (values.length() == 0)
      return {};

    std::vector<std::string> list{};
    size_t commapos = values.find_first_of(',');
    while (commapos != std::string::npos) {
      std::string value = values.substr(0, commapos);
      std::string newvalues = values.substr(commapos + 1, std::string::npos);
      values = newvalues;
      commapos = values.find_first_of(',');

      list.push_back(value);
    }
    list.push_back(values);

    std::vector<T> out_list{};
    for (const auto &val : list) {
      out_list.push_back(string_to_value<T>(val));
    }
    return out_list;
  }

  private:
  // Name-Value
  std::unordered_map<std::string, std::string> _args{};

  std::vector<std::string> _danling_args{};

  bool is_dangling(const std::string &arg) const
  {
    return arg.length() > 0 && arg.at(0) != '-';
  }

  Argument get_argument(const std::string &arg) const
  {
    if (arg.length() == 0)
      return {};

    size_t eqpos = arg.find('=');
    if (eqpos == std::string::npos) {
      return {
        .name = arg.substr(1, std::string::npos),
        .value = "",
      };
    }
    std::string name = arg.substr(1, eqpos - 1);

    std::string value = arg.substr(eqpos + 1, std::string::npos);
    return { .name = name, .value = value };
  }

  // Haha, Scary SFINAE!!!
  template <
    typename T,
    typename U = typename std::enable_if<
      std::__or_<std::is_floating_point<T>, std::is_integral<T>,
                 std::is_same<T, std::string>, std::is_same<T, bool> >::value,
      T>::type>
  constexpr T string_to_value(const std::string &str) const
  {
    if constexpr (std::is_floating_point<T>()) {
      return std::atof(str.c_str());
    } else if constexpr (std::is_integral<T>()) {
      return std::atoll(str.c_str());
    } else if constexpr (std::is_same<T, std::string>()) {
      return str;
    } else if constexpr (std::is_same<T, bool>()) {
      char lowercase[8192] = { 0 };
      for (size_t i = 0; i < str.size(); i++) {
        lowercase[i] = tolower(str.at(i));
      }

      return strncmp(lowercase, "true", sizeof(lowercase)) == 0 ||
             strncmp(lowercase, "t", sizeof(lowercase)) == 0 ||
             strncmp(lowercase, "1", sizeof(lowercase)) == 0;
    }
    return {};
  }
};
