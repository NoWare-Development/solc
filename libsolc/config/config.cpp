#include "config/config.hpp"

namespace solc
{

void Config::set_include_paths(const std::vector<std::string> &paths)
{
  _include_paths = std::move(paths);
}

const std::vector<std::string> &Config::get_include_paths() const
{
  return _include_paths;
}

void Config::set_link_lib_search_paths(std::vector<std::string> paths)
{
  _link_lib_search_paths = std::move(paths);
}

const std::vector<std::string> &Config::get_link_lib_search_paths() const
{
  return _link_lib_search_paths;
}

void Config::set_link_libs(std::vector<std::string> libs)
{
  _link_libs = std::move(libs);
}

const std::vector<std::string> Config::get_link_libs() const
{
  return _link_libs;
}

void Config::set_optimization_level(size_t level)
{
  _optimization_level = level;
}

size_t Config::get_optimization_level() const
{
  return _optimization_level;
}

void Config::set_compiler_flags(Config::CompilerFlags flags)
{
  _compiler_flags = flags;
}

Config::CompilerFlags Config::get_compiler_flags() const
{
  return _compiler_flags;
}

void Config::set_compiler_action(Config::CompilerAction act)
{
  _compiler_action = act;
}

Config::CompilerAction Config::get_compiler_action() const
{
  return _compiler_action;
}

void Config::set_machine_arch(MachineArch arch)
{
  _machine_arch = arch;
}

Config::MachineArch Config::get_machine_arch() const
{
  return _machine_arch;
}

Config::MachineArch Config::get_default_machine_arch() const
{
  // TODO: other architectures
  return MachineArch::ARCH_AMD64;
}

}
