#include "argparser.hpp"
#include "config/config.hpp"
#include "error_handler/error_handler.hpp"
#include "info/info.hpp"
#include "util/util.hpp"
#include <fstream>
#include <iostream>
#include <lexer/lexer.hpp>
#include <parser/parser.hpp>
#include <sa/sa.hpp>
#include <sstream>

static void handle_arguments(ArgParser &argparser);
static void get_arch(ArgParser &argparser);

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

    solc::SemanticAnalyzer sa{};
    sa.analyze(root);
    auto sa_errors = sa.get_errors();
    if (!sa_errors.empty()) {
      handler.handle_sa_errors(sa_errors);
      return -6;
    }
  }

  return 0;
}

static void handle_arguments(ArgParser &argparser)
{
  auto optlevel = argparser.get_argument_value<size_t>("opt");
  solc::Config::the().set_optimization_level(optlevel);

  auto includes = argparser.get_argument_value_list<std::string>("include");
  solc::Config::the().set_include_paths(includes);

  auto libpaths = argparser.get_argument_value_list<std::string>("libpath");
  solc::Config::the().set_link_lib_search_paths(libpaths);

  auto libs = argparser.get_argument_value_list<std::string>("linkwith");
  solc::Config::the().set_link_libs(libs);

  auto has_nostdlib = argparser.has_argument("nostdlib");
  if (has_nostdlib) {
    solc::Config::the().set_compiler_flag(
      solc::Config::CompilerFlag::COMPILER_FLAG_NOSTDLIB);
  }

  auto has_freestanding = argparser.has_argument("freestanding");
  if (has_freestanding) {
    solc::Config::the().set_compiler_flag(
      solc::Config::CompilerFlag::COMPILER_FLAG_FREESTANDING);
  }

  // Get arch
  get_arch(argparser);
}

static void get_arch(ArgParser &argparser)
{
  bool arch_set = false;

  auto arch_x86 = argparser.has_argument("mi386");
  if (arch_x86) {
    if (arch_set) {
      std::cout << "Argument -mi386 will override previously set target "
                   "architecture option.\n";
    }

    solc::Config::the().set_output_arch(solc::Config::OutputArch::ARCH_X86);
    arch_set = true;
  }

  auto arch_x64 = argparser.has_argument("mamd64");
  if (arch_x64) {
    if (arch_set) {
      std::cout << "Argument -mamd64 will override previously set target "
                   "architecture option.\n";
    }
    solc::Config::the().set_output_arch(solc::Config::OutputArch::ARCH_AMD64);
    arch_set = true;
  }

  auto arch_arm64 = argparser.has_argument("marm64");
  if (arch_arm64) {
    if (arch_set) {
      std::cout << "Argument -marm64 will override previously set target "
                   "architecture option.\n";
    }
    solc::Config::the().set_output_arch(solc::Config::OutputArch::ARCH_ARM64);
    arch_set = true;
  }

  if (!arch_set) {
    solc::Config::the().set_output_arch(solc::Config::the().get_default_arch());
  }
}
