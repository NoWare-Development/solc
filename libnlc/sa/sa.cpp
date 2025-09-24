#include "sa/sa.hpp"
#include "libnlc/util.hpp"
#include "symbols/symboltable.hpp"
#include <memory>

namespace nlc
{

std::unique_ptr<SymbolTable>
SemanticAnalyzer::analyze (const AST &root)
{
  _scope_stack = new ScopeStack ();
  _symbol_table = std::make_unique<SymbolTable> ();

  _scope_stack->push (Scope (Scope::Kind::GLOBAL));

  if (root.type != ASTType::PROG)
    return nullptr;

  analyze_prog (root);

  delete _scope_stack;

  return std::move (_symbol_table);
}

void
SemanticAnalyzer::analyze_prog (const AST &prog)
{
  for (const auto &child : prog.children)
    {
      switch (child.type)
        {
        case ASTType::TYPEDEF:
          analyze_typedef (child);
          break;

        default:
          NOREACH ();
        }
    }
}

const std::vector<SemanticAnalyzer::SAError> &
SemanticAnalyzer::get_errors () const
{
  return _errors;
}

}
