#include "semantic-analyzer/sa.hpp"

namespace nlc::sa
{

void
SemanticAnalyzer::analyze_vardecl (const AST &vardecl)
{
  std::string name = vardecl.value;
  if (name.length () == 0)
    {
      add_error (vardecl.token_position,
                 SAError::SA_ERR_TYPE_NO_NAME_FOR_VARIABLE);
      return;
    }

  Type type_of_variable = get_type_from_type_ast (vardecl.children.at (0));
  if (type_of_variable.type == BuiltinType::BUILTIN_TYPE_UNK)
    return;

  if (!verify_comptime_array (vardecl.children.at (0)))
    return;

  if (variable_is_defined (name))
    {
      add_error (vardecl.token_position,
                 SAError::SA_ERR_TYPE_VARIABLE_ALREADY_DECLARED);
      return;
    }

  Variable out = {
    .type = type_of_variable,
  };
  _scope_stack.top ().variable_table[name] = out;
}

}
