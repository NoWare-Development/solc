#include "argparser.hpp"
#include "config/config.hpp"
#include "error_handler/error_handler.hpp"
#include "info/info.hpp"
#include "util/util.hpp"
#include <fstream>
#include <iostream>
#include <lexer/lexer.hpp>
#include <parser/parser.hpp>
#include <sstream>

static void handle_arguments(const ArgParser &argparser);
static void get_arch(const ArgParser &argparser);
static void get_action(const ArgParser &argparser);

int main(int argc, char **argv)
{
  ArgParser argparser(argc, argv);
  if (argparser.has_argument("help")) {
    info::display_help();
    return 0;
  } else if (argparser.has_argument("version")) {
    info::display_version();
    return 0;
  }

  handle_arguments(argparser);

  auto sources = argparser.get_dangling_arguments();
  if (sources.empty()) {
    std::cout << escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD)
              << "solc: " << escape_color(ESCColor::ESCCOLOR_RED)
              << "fatal error: " << escape_reset()
              << "no input files\ncompilation terminated.\n";
    return -1;
  }

  for (const auto &srcfile : sources) {
    std::ifstream file(srcfile);
    if (!file.is_open()) {
      std::cout << escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD)
                << "solc: " << escape_color(ESCColor::ESCCOLOR_RED)
                << "fatal error: " << escape_reset() << "failed to open file \""
                << srcfile << "\"\n";
      return -2;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string src = buffer.str();

    ErrorHandler handler(srcfile, src);

    solc::Lexer lexer{};
    auto tokens = lexer.tokenize(src);

    // TODO: remove me
    for (size_t i = 0; i < tokens.size(); i++) {
      std::cout << "(" << i << ") " << tokens.at(i).to_string() << '\n';
    }
    handler.add_tokens(tokens);
    if (!handler.handle_tokens()) {
      return -3;
    }

    solc::Parser parser(tokens);
    auto root = parser.parse();
    std::cout << root.to_string() << '\n';
    auto errors = parser.get_errors();
    handler.add_parser_errors(errors);
    if (!handler.handle_parser_errors())
      return -4;
    else if (!handler.handle_invalid_expressions(root))
      return -5;
  }

  return 0;
}

static void handle_arguments(const ArgParser &argparser)
{
  auto cfg = &solc::Config::the();

  auto optlevel = argparser.get_argument_value<size_t>("opt");
  cfg->set_optimization_level(optlevel);

  auto includes = argparser.get_argument_value_list<std::string>("include");
  cfg->set_include_paths(includes);

  auto libpaths = argparser.get_argument_value_list<std::string>("libpath");
  cfg->set_link_lib_search_paths(libpaths);

  auto libs = argparser.get_argument_value_list<std::string>("linkwith");
  cfg->set_link_libs(libs);

  // Get compiler flags
  solc::Config::CompilerFlags flags = solc::Config::COMPILER_FLAG_NONE;

  if (argparser.has_argument("nostdlib")) {
    flags |= solc::Config::COMPILER_FLAG_NOSTDLIB;
  }
  if (argparser.has_argument("freestanding")) {
    flags |= solc::Config::COMPILER_FLAG_FREESTANDING;
  }

  cfg->set_compiler_flags(flags);

  get_arch(argparser);
  get_action(argparser);
}

static void get_arch(const ArgParser &argparser)
{
  bool arch_set = false;

  if (argparser.has_argument("mi386")) {
    solc::Config::the().set_machine_arch(solc::Config::MachineArch::ARCH_X86);
    arch_set = true;
  }
  if (argparser.has_argument("mamd64")) {
    if (arch_set) {
      std::cout << "Argument -mamd64 will override previously set target "
                   "architecture option.\n";
    }
    solc::Config::the().set_machine_arch(solc::Config::MachineArch::ARCH_AMD64);
    arch_set = true;
  }
  if (argparser.has_argument("marm64")) {
    if (arch_set) {
      std::cout << "Argument -marm64 will override previously set target "
                   "architecture option.\n";
    }
    solc::Config::the().set_machine_arch(solc::Config::MachineArch::ARCH_ARM64);
    arch_set = true;
  }
}

static void get_action(const ArgParser &argparser)
{
  auto selected_action =
    solc::Config::CompilerAction::COMPILE_AND_LINK_EXECUTABLE;

  bool selected = false;
  if (argparser.has_argument("shared")) {
    selected_action = solc::Config::CompilerAction::COMPILE_AND_LINK_SHARED;
    selected = true;
  }
  if (argparser.has_argument("static")) {
    selected_action = solc::Config::CompilerAction::COMPILE_AND_LINK_STATIC;
    if (selected) {
      std::cout << "Argument -static will override previous compiler action\n";
    }
    selected = true;
  }
  if (argparser.has_argument("C")) {
    selected_action = solc::Config::CompilerAction::COMPILE;
    if (selected) {
      std::cout << "Argument -C will override previous compiler action\n";
    }
    selected = true;
  }
  if (argparser.has_argument("ir")) {
    selected_action = solc::Config::CompilerAction::GET_IR;
    if (selected) {
      std::cout << "Argument -ir will override previous compiler action\n";
    }
    selected = true;
  }
  if (argparser.has_argument("S")) {
    selected_action = solc::Config::CompilerAction::GET_ASSEMBLY;
    if (selected) {
      std::cout << "Argument -S will override previous compiler action\n";
    }
    selected = true;
  }
}
