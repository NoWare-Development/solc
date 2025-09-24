#include "sa/sa.hpp"

namespace nlc
{

void
SemanticAnalyzer::analyze_typedef (const AST &tdef)
{
  const auto &alias_name = tdef.value;
  auto type = get_type_from_type_ast (tdef.children.at (0));

  if (does_type_exist (alias_name))
    {
      add_error (SAErrorType::TYPE_REDEF, tdef.token_position);
      return;
    }

  if (type == nullptr)
    {
      add_error (SAErrorType::UNDEFINED_TYPE,
                 tdef.children.at (0).token_position);
      return;
    }

  _scope_stack->top ()->add_alias (alias_name, type);
}

}
