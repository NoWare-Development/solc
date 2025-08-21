#include "semantic-analyzer/sa.hpp"
#include <cstddef>

namespace nlc::sa
{

void
SemanticAnalyzer::analyze_vardef (const AST &vardef)
{
  std::string name = vardef.value;
  if (name.length () == 0)
    {
      add_error (vardef.token_position,
                 SAError::SA_ERR_TYPE_NO_NAME_FOR_VARIABLE);
      return;
    }

  Type type_of_variable = get_type_from_type_ast (vardef.children.at (0));
  Type type_of_expr = get_type_from_expr_ast (vardef.children.at (1));

  if (!can_convert_type (type_of_expr, type_of_variable))
    {
      std::vector<size_t> poses = {
        vardef.children.at (1).token_position,
        vardef.token_position,
      };
      add_error (poses, SAError::SA_ERR_TYPE_CANNOT_CONVERT_TYPES_IN_VARDEF);
      return;
    }

  Variable out = {
    .type = type_of_variable,
  };
  _scope_stack.top ().variable_table[name] = out;
}

}
