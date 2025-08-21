#include "libnlc/util.hpp"
#include "semantic-analyzer/sa.hpp"

namespace nlc::sa
{

Type
SemanticAnalyzer::get_type_from_type_ast (const AST &type_ast)
{
  Type out_type{};
  int pointer_count = 0;

  auto current = &type_ast;
  while (current != nullptr)
    {
      if (current->type == ASTType::AST_TYPE_ARRAY)
        {
          pointer_count += 1;
          size_t children_type_pos = -1;
          switch (current->children.size ())
            {
            case 1:
              children_type_pos = 0;
              break;
            case 2:
              children_type_pos = 1;
              break;
            default:
              add_error (current->token_position,
                         SAError::ErrType::
                             SA_ERR_TYPE_WRONG_NUMBER_OF_ARRAYS_CHILDREN);
              return {};
            }
          current = &current->children.at (children_type_pos);
          continue;
        }
      else if (current->type == ASTType::AST_TYPE_POINTER)
        {
          pointer_count += 1;
          if (current->children.size () != 1)
            {
              add_error (current->token_position,
                         SAError::ErrType::
                             SA_ERR_TYPE_WRONG_NUMBER_OF_POINTERS_CHILDREN);
              return {};
            }
          current = &current->children.at (0);
          continue;
        }
      else if (current->type == ASTType::AST_TYPE_PLAIN)
        {
          Type resolved_type
              = resolve_plain_type (current->value, current->token_position);
          out_type.pointer_count = resolved_type.pointer_count;
          out_type.type = resolved_type.type;
          out_type.type_name = resolved_type.type_name;
          break;
        }
      else if (current->type == ASTType::AST_TYPE_FUNCPTR)
        {
          TODO ("Function pointers");
        }
      else
        {
          add_error (current->token_position,
                     SAError::ErrType::SA_ERR_TYPE_TYPE_HAS_NO_PLAIN_TYPE);
          return {};
        }
    }

  out_type.pointer_count += pointer_count;

  return out_type;
}

Type
SemanticAnalyzer::resolve_plain_type (const std::string &type,
                                      size_t plain_type_pos)
{
  Type out{};

  if (typestr_to_builtin_type.find (type) != typestr_to_builtin_type.end ())
    {
      out.type = typestr_to_builtin_type.at (type);
      return out;
    }

  // TODO: add tests for others

  add_error (plain_type_pos,
             SAError::ErrType::SA_ERR_TYPE_TYPE_IS_NOT_DEFINED);
  return {};
}

}
