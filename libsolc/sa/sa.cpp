#include "sa/sa.hpp"
#include "config/config.hpp"
#include "libsolc/util.hpp"
#include "sa/scope.hpp"
#include "symbols/symbol.hpp"
#include "symbols/symbolscope.hpp"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

namespace solc
{

SemanticAnalyzer::SemanticAnalyzer()
{
  populate_architecture_dependent_types();
}

void SemanticAnalyzer::analyze(const AST &root)
{
  _symbol_scope_bindings[(void *)&root] = SymbolScope(nullptr);
  _global_symbol_scope = &_symbol_scope_bindings.at((void *)&root);
  if (root.type != ASTType::PROG)
    return;

  analyze_prog(root);

  _global_symbol_scope = nullptr;
}

void SemanticAnalyzer::analyze_prog(const AST &prog)
{
  struct_asts.clear();
  union_asts.clear();
  enum_asts.clear();
  func_asts.clear();

  _current_symbol_scope = _global_symbol_scope;
  generate_symbol_scope_bindings(prog);
  if (!_errors.empty())
    return;

  generate_aliases(prog);
  if (!_errors.empty())
    return;

  populate_symbols();

  static std::string (*symbol_scope_to_string)(const SymbolScope &) =
    [](const SymbolScope &scope) {
      std::string out{};
      if (scope.get_parent() != nullptr) {
        std::stringstream ss;
        ss << scope.get_parent();
        out = " -> " + ss.str() + symbol_scope_to_string(*scope.get_parent());
      }

      return out;
    };
  std::cout << "Symbol scopes:\n";
  for (const auto &kv : _symbol_scope_bindings) {
    std::cout << '[' << kv.first << "]" << symbol_scope_to_string(kv.second)
              << '\n';
  }

  _scope_stack = new ScopeStack;
  _scope_stack->push(Scope(Scope::Kind::GLOBAL));
  delete _scope_stack;
}

const std::vector<SemanticAnalyzer::SAError> &
SemanticAnalyzer::get_errors() const
{
  return _errors;
}

void SemanticAnalyzer::populate_architecture_dependent_types()
{
  std::shared_ptr<Type> unsigned_max_len;
  std::shared_ptr<Type> signed_max_len;

  switch (Config::the().get_output_arch()) {
  case Config::OutputArch::ARCH_AMD64:
  case Config::OutputArch::ARCH_ARM64:
    unsigned_max_len = _basic_types.at("ulong");
    signed_max_len = _basic_types.at("long");
    break;

  case Config::OutputArch::ARCH_X86:
    unsigned_max_len = _basic_types.at("uint");
    signed_max_len = _basic_types.at("int");
    break;
  }

  _architecture_dependent_types["size_t"] = unsigned_max_len;
  _architecture_dependent_types["uptr"] = unsigned_max_len;
  _architecture_dependent_types["off_t"] = signed_max_len;
  _architecture_dependent_types["sptr"] = signed_max_len;
}

}
