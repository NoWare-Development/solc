#include "sa/sa.hpp"

namespace nlc
{

void
SemanticAnalyzer::analyze_vardecl (const AST &vardecl, ModifierFlags flags)
{
  const auto &name = vardecl.value;
  auto type = get_type_from_type_ast (vardecl.children.at (0));
  if (type == nullptr)
    return;

  for (const auto &scope : *_scope_stack)
    {
      if (scope.has_variable (name))
        {
          add_error (SAErrorType::VAR_REDECL, vardecl.token_position);
          return;
        }
    }

  _scope_stack->top ()->add_variable (name,
                                      Variable::create (type, false, flags));
}

}
