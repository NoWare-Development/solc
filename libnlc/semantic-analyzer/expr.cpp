#include "libnlc/util.hpp"
#include "parser/ast.hpp"
#include "semantic-analyzer/sa.hpp"
#include "semantic-analyzer/types.hpp"
#include <cstddef>

namespace nlc::sa
{

Type
SemanticAnalyzer::get_type_from_expr_ast (const AST &expr_ast)
{
  auto exprtype = expr_ast.type;
  switch (exprtype)
    {
    case ASTType::AST_EXPR_OPERAND_NUM:
      {
        Type out_type = { .type = BuiltinType::BUILTIN_TYPE_INT };

        if (expr_ast.children.size () > 0)
          {
            // Type specifier
            if (expr_ast.children.size () != 1)
              {
                add_error (expr_ast.token_position,
                           SAError::ErrType::
                               SA_ERR_TYPE_WRONG_NUMBER_OF_INTLIT_CHILDREN);
                return {};
              }

            const auto &child = expr_ast.children.at (0);
            auto lowercase_typespec = util::string_lowercase (child.value);
            if (!is_string_a_typespec (lowercase_typespec))
              {
                add_error (child.token_position,
                           SAError::ErrType::SA_ERR_TYPE_INVALID_TYPESPEC);
                return {};
              }

            auto typespec_type
                = typespecstr_to_builtin_type.at (lowercase_typespec);
            out_type.type = typespec_type;
          }

        return out_type;
      }
    case ASTType::AST_EXPR_OPERAND_NUMFLOAT:
      {
        Type out_type{ .type = BuiltinType::BUILTIN_TYPE_DOUBLE };

        if (expr_ast.children.size () > 0)
          {
            // Type specifier
            if (expr_ast.children.size () != 1)
              {
                add_error (expr_ast.token_position,
                           SAError::ErrType::
                               SA_ERR_TYPE_WRONG_NUMBER_OF_FLOATLIT_CHILDREN);
                return {};
              }

            const auto &child = expr_ast.children.at (0);
            auto lowercase_typespec = util::string_lowercase (child.value);
            if (!is_string_a_typespec (lowercase_typespec))
              {
                add_error (child.token_position,
                           SAError::ErrType::SA_ERR_TYPE_INVALID_TYPESPEC);
                return {};
              }

            auto typespec_type
                = typespecstr_to_builtin_type.at (lowercase_typespec);
            switch (typespec_type)
              {
              case BuiltinType::BUILTIN_TYPE_FLOAT:
              case BuiltinType::BUILTIN_TYPE_DOUBLE:
                out_type.type = typespec_type;
                break;

              default:
                add_error (
                    child.token_position,
                    SAError::ErrType::
                        SA_ERR_TYPE_CANNOT_CONVERT_FLOAT_TO_INTTYPE_TYPESPEC);
                return {};
              }
          }

        return out_type;
      }

    case ASTType::AST_EXPR_OPERAND_SYMBOL:
      return (Type){ .type = BuiltinType::BUILTIN_TYPE_CHAR };

    case ASTType::AST_EXPR_OPERAND_STRING:
      return (Type){ .pointer_count = 1,
                     .type = BuiltinType::BUILTIN_TYPE_CHAR };

    case ASTType::AST_EXPR_OPERAND_IDENTIFIER:
      {
        bool found;
        auto out = search_for_variable_type (expr_ast.value, found);
        if (!found)
          {
            add_error (expr_ast.token_position,
                       SAError::ErrType::SA_ERR_TYPE_UNDECLARED_VARIABLE);
            return {};
          }

        return out;
      }

    case ASTType::AST_EXPR_OPERAND_CALL:
      TODO ("Calls");
      break;

    case ASTType::AST_EXPR_OPERAND_CAST_TO:
      {
        auto type_to_cast = get_type_from_type_ast (expr_ast.children.at (0));
        auto type_of_operand
            = get_type_from_expr_ast (expr_ast.children.at (1));

        if (!can_convert_type (type_of_operand, type_to_cast))
          {
            add_error (
                expr_ast.children.at (1).token_position,
                SAError::ErrType::SA_ERR_TYPE_CANNOT_CONVERT_TYPES_IN_CAST);
            return {};
          }

        return type_to_cast;
      }

    case ASTType::AST_EXPR:
      {
        Type out_type{};

        if (expr_ast.children.size () != 3)
          {
            add_error (
                expr_ast.token_position,
                SAError::ErrType::SA_ERR_TYPE_WRONG_NUMBER_OF_EXPR_CHILDREN);
            return {};
          }

        auto lhs_type = get_type_from_expr_ast (expr_ast.children.at (0));
        auto rhs_type = get_type_from_expr_ast (expr_ast.children.at (2));
        if (!can_convert_type (rhs_type, lhs_type))
          {
            std::vector<size_t> poses = {
              expr_ast.children.at (2).token_position,
              expr_ast.children.at (0).token_position,
            };
            add_error (
                poses,
                SAError::ErrType::SA_ERR_TYPE_CANNOT_CONVERT_TYPES_IN_EXPR);
            return {};
          }

        // TODO: check for different operations on types that don't support for
        // those.

        out_type = lhs_type;

        return out_type;
      }

    case ASTType::AST_PREFIX_EXPR:
      {
        auto out_type = get_type_from_expr_ast (expr_ast.children.at (0));
        for (size_t i = 1; i < expr_ast.children.size (); i++)
          {
            const auto &op = expr_ast.children.at (i);

            switch (op.type)
              {
              case ASTType::AST_EXPR_PREFIX_OPERATOR_ADDRESS:
                out_type.pointer_count += 1;
                break;

              case ASTType::AST_EXPR_PREFIX_OPERATOR_NOT:
                {
                  if (is_type_is_integer (out_type))
                    {
                      out_type.type = BuiltinType::BUILTIN_TYPE_BOOL;
                      out_type.type_name = {};
                      out_type.pointer_count = 0;
                      break;
                    }

                  TODO ("Handle structs, unions and floats");
                }
                break;

              case ASTType::AST_EXPR_PREFIX_OPERATOR_DEREF:
                {
                  if (out_type.pointer_count <= 0)
                    {
                      add_error (
                          op.token_position,
                          SAError::ErrType::
                              SA_ERR_TYPE_DEREFERENCING_NONPOINTER_VALUE);
                      return {};
                    }

                  out_type.pointer_count -= 1;
                }
                break;

              case ASTType::AST_EXPR_PREFIX_OPERATOR_BNOT:
                {
                  if (!is_type_is_integer (out_type))
                    {
                      add_error (op.token_position,
                                 SAError::ErrType::
                                     SA_ERR_TYPE_BINARY_NOT_ON_NON_INTEGER);
                      return {};
                    }

                  if (out_type.pointer_count > 0)
                    {
                      add_error (op.token_position,
                                 SAError::ErrType::
                                     SA_ERR_TYPE_BINARY_NOT_ON_A_POINTER);
                      return {};
                    }

                  if (out_type.type == BuiltinType::BUILTIN_TYPE_ENUM)
                    {
                      out_type.type = BuiltinType::BUILTIN_TYPE_INT;
                    }
                }
                break;

              case ASTType::AST_EXPR_PREFIX_OPERATOR_NEG:
                {
                  if (out_type.pointer_count > 0)
                    {
                      add_error (
                          op.token_position,
                          SAError::ErrType::SA_ERR_TYPE_NEGATIVE_OF_POINTER);
                      return {};
                    }

                  if (!is_type_is_integer (out_type)
                      && !is_type_is_float (out_type))
                    {
                      switch (out_type.type)
                        {
                        case BuiltinType::BUILTIN_TYPE_VOID:
                          add_error (
                              op.token_position,
                              SAError::ErrType::SA_ERR_TYPE_NEGATIVE_OF_VOID);
                          break;
                        case BuiltinType::BUILTIN_TYPE_STRUCT:
                          add_error (op.token_position,
                                     SAError::ErrType::
                                         SA_ERR_TYPE_NEGATIVE_OF_STRUCT);
                          break;
                        case BuiltinType::BUILTIN_TYPE_UNION:
                          add_error (
                              op.token_position,
                              SAError::ErrType::SA_ERR_TYPE_NEGATIVE_OF_UNION);
                          break;

                        default:
                          NEVERREACH ();
                        }
                    }
                }
                break;

              default:
                break;
              }
          }

        return out_type;
      }

    default:
      break;
    }

  return {};
}

}
