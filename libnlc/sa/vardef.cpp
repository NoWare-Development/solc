#include "sa/sa.hpp"
#include "symbols/symbol.hpp"
#include "types.hpp"

namespace nlc
{

void
SemanticAnalyzer::analyze_vardef (const AST &vardef, ModifierFlags flags)
{
  const auto &name = vardef.value;
  auto vardef_type = get_type_from_type_ast (vardef.children.at (0));
  if (vardef_type == nullptr)
    return;

  auto definition = vardef.children.at (1);
  if (definition.type == ASTType::INITLIST)
    {
      if (!verify_type_of_initlist (definition, vardef_type,
                                    vardef.children.at (0).token_position))
        return;
    }
  else
    {
      auto expr_type = get_type_from_expr_ast (vardef.children.at (1));
      if (expr_type == nullptr)
        return;

      if (!can_convert_types (expr_type, vardef_type))
        {
          add_error (SAErrorType::CANNOT_CONVERT_TYPES_EXPR,
                     vardef.children.at (1).token_position);
          return;
        }
    }

  for (const auto &scope : *_scope_stack)
    {
      if (scope.has_variable (name))
        {
          add_error (SAErrorType::VAR_REDECL, vardef.token_position);
          return;
        }
    }

  _scope_stack->top ()->add_variable (
      name, Variable::create (vardef_type, true, flags));
}

}
