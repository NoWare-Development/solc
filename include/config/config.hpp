#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace solc
{

// Singleton class that stores compiler's configuration.
class Config {
  public:
  ~Config() = default;

  Config(const Config &) = delete;
  Config(Config &&) = delete;
  Config &operator=(const Config &) = delete;
  Config &operator=(Config &&) = delete;

  // Returns instance of Config class.
  static Config &the()
  {
    static Config _instance;
    return _instance;
  }

  // Sets include paths.
  void set_include_paths(const std::vector<std::string> &paths);

  // Returns include paths.
  const std::vector<std::string> &get_include_paths() const;

  // Sets the paths the compiler will search for libraries when linking
  // with them.
  void set_link_lib_search_paths(std::vector<std::string> paths);

  // Returns the paths the compiler will search for libraries when linking
  // with them.
  const std::vector<std::string> &get_link_lib_search_paths() const;

  // Sets libraries that compiler will link executable with.
  void set_link_libs(std::vector<std::string> libs);

  // Returns libraries that compiler will link executable with.
  const std::vector<std::string> get_link_libs() const;

  // Sets optimization level.
  void set_optimization_level(size_t level);

  // Returns optimization levels.
  size_t get_optimization_level() const;

  enum CompilerFlagBits {
    COMPILER_FLAG_NONE = 0,

    // -fnostdlib: do not use standard library when compiling executable.
    COMPILER_FLAG_NOSTDLIB = 1 << 0,

    // -ffreestanding: do not use C runtime.
    COMPILER_FLAG_FREESTANDING = 1 << 1,
  };
  using CompilerFlags = uint32_t;

  // Sets compiler flags.
  void set_compiler_flags(CompilerFlags flags);

  // Returns compiler flags.
  CompilerFlags get_compiler_flags() const;

  enum class CompilerAction {
    // default behaviour.
    COMPILE_AND_LINK_EXECUTABLE,

    // -shared: compile and link shared library.
    COMPILE_AND_LINK_SHARED,

    // -static: compile and link static library.
    COMPILE_AND_LINK_STATIC,

    // -C: compile to object file.
    COMPILE,

    // -ir: compile to LLVM's Intermediate Representation language.
    GET_IR,

    // -S: compile to GNU assembly.
    GET_ASSEMBLY,
  };

  // Sets compiler action.
  void set_compiler_action(CompilerAction act);

  // Returns compiler action.
  CompilerAction get_compiler_action() const;

  enum MachineArch {
    // -m386: compile for x86 architecture.
    ARCH_X86,

    // -mamd64: compile for AMD64 (x86_64) architecture.
    ARCH_AMD64,

    // -marm64: compile for ARM64 architecture.
    ARCH_ARM64,
  };

  // Sets machine architecture.
  void set_machine_arch(MachineArch arch);

  // Returns machine architecture.
  MachineArch get_machine_arch() const;

  // Returns machine architecture compiler was built for.
  MachineArch get_default_machine_arch() const;

  private:
  Config() = default;

  std::vector<std::string> _include_paths{};

  std::vector<std::string> _link_lib_search_paths{};
  std::vector<std::string> _link_libs{};

  size_t _optimization_level{};

  uint32_t _compiler_flags{};

  CompilerAction _compiler_action{};
  MachineArch _machine_arch{ get_default_machine_arch() };
};

}
