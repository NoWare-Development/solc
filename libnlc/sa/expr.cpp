#include "libnlc/util.hpp"
#include "sa/sa.hpp"
#include "types.hpp"

namespace nlc
{

SemanticAnalyzer::SAError
SemanticAnalyzer::is_expr_known_at_comptime (const AST &expr) const
{
  switch (expr.type)
    {
    case ASTType::EXPR_OPERAND_NUM:
    case ASTType::EXPR_OPERAND_NUMFLOAT:
    case ASTType::EXPR_OPERAND_SYMBOL:
      return SAError::success ();

    case ASTType::EXPR_OPERAND_IDENTIFIER:
      {
        if (!is_valid_literal (expr.value))
          {
            return SAError (SAErrorType::IDENTIFIER_IN_CONST_EXPR,
                            expr.token_position);
          }
      }
      break;

    case ASTType::EXPR_OPERAND_ARRAY_ELEMENT:
      return SAError (SAErrorType::ARRAY_ELEM_IN_CONST_EXPR,
                      expr.token_position);

    case ASTType::EXPR_OPERAND_ACCESS_MEMBER:
      return SAError (SAErrorType::ACCESS_MEMBER_IN_CONST_EXPR,
                      expr.token_position);

    case ASTType::EXPR_OPERAND_CAST_TO:
      return is_expr_known_at_comptime (expr.children.at (1));

    case ASTType::EXPR_OPERAND_CALL:
      return SAError (SAErrorType::CALL_IN_CONST_EXPR, expr.token_position);

    case ASTType::EXPR_OPERAND_STRING:
      return SAError (SAErrorType::STRING_IN_CONST_EXPR, expr.token_position);

    case ASTType::EXPR:
      {
        if (expr.children.size () > 1)
          {
            auto lhs_known = is_expr_known_at_comptime (expr.children.at (0));
            if (lhs_known.type != SAErrorType::NONE)
              return lhs_known;

            auto rhs_known = is_expr_known_at_comptime (expr.children.at (2));
            if (rhs_known.type != SAErrorType::NONE)
              return rhs_known;
          }
        else
          {
            auto operand_known
                = is_expr_known_at_comptime (expr.children.at (0));
            if (operand_known.type != SAErrorType::NONE)
              return operand_known;
          }

        return SAError::success ();
      }

    case ASTType::PREFIX_EXPR:
      {
        // Check for operand
        auto operand_known = is_expr_known_at_comptime (expr.children.at (0));
        if (operand_known.type != SAErrorType::NONE)
          return operand_known;

        // Check for operators
        for (size_t i = 1; i < expr.children.size (); i++)
          {
            const auto &prefix_operator = expr.children.at (i);
            switch (prefix_operator.type)
              {
              case ASTType::EXPR_PREFIX_OPERATOR_DEREF:
                return SAError (SAErrorType::DEREF_IN_CONST_EXPR,
                                prefix_operator.token_position);

              case ASTType::EXPR_PREFIX_OPERATOR_ADDRESS:
                return SAError (SAErrorType::ADDRESS_IN_CONST_EXPR,
                                prefix_operator.token_position);

              default:
                break;
              }
          }

        return SAError::success ();
      }

    case ASTType::NAMESPACE:
      TODO ("Namespace identifier");

    default:
      break;
    }

  return SAError::success ();
}

std::shared_ptr<Type>
SemanticAnalyzer::get_type_from_expr_ast (const AST &expr)
{
  switch (expr.type)
    {
    case ASTType::EXPR_OPERAND_NUM:
      {
        if (!expr.children.empty ())
          {
            const auto &typespec = expr.children.at (0);
            if (!is_valid_typespec (typespec.value))
              {
                add_error (SAErrorType::UNKNOWN_NUMERIC_TYPESPEC,
                           typespec.token_position);
                return nullptr;
              }

            auto typespec_type = get_typespec (typespec.value);
            return typespec_type;
          }
        return get_basic_type ("int");
      }
      break;

    case ASTType::EXPR_OPERAND_NUMFLOAT:
      {
        if (!expr.children.empty ())
          {
            const auto &typespec = expr.children.at (0);
            if (!is_valid_typespec (typespec.value))
              {
                add_error (SAErrorType::UNKNOWN_NUMERIC_TYPESPEC,
                           typespec.token_position);
                return nullptr;
              }

            auto typespec_type = get_typespec (typespec.value);
            if (typespec_type->is_integer ())
              {
                add_error (SAErrorType::INT_TYPESPEC_ON_FLOAT,
                           typespec.token_position);
                return nullptr;
              }

            return typespec_type;
          }
        return get_basic_type ("double");
      }
      break;

    case ASTType::EXPR_OPERAND_SYMBOL:
      return get_basic_type ("char");

    case ASTType::EXPR_OPERAND_IDENTIFIER:
      {
        if (is_valid_literal (expr.value))
          return get_literal_type (expr.value);

        // TODO: Check for imported symbols.
        for (const auto &scope : *_scope_stack)
          {
            if (scope.has_variable (expr.value))
              {
                return scope.get_variable (expr.value)->type->copy ();
              }
          }

        add_error (SAErrorType::UNDECLARED_VARIABLE, expr.token_position);
        return nullptr;
      }
      break;

    case ASTType::EXPR_OPERAND_STRING:
      return Type::create_basic (BuiltinType::CHAR, 1);

    case ASTType::EXPR_OPERAND_ARRAY_ELEMENT:
      {
        std::shared_ptr<Type> out_type{};

        for (const auto &scope : *_scope_stack)
          {
            if (scope.has_variable (expr.value))
              {
                out_type = scope.get_variable (expr.value)->type->copy ();
                break;
              }
          }
        if (out_type == nullptr)
          {
            add_error (SAErrorType::UNDECLARED_VARIABLE, expr.token_position);
            return nullptr;
          }

        if (!out_type->is_pointer ())
          {
            add_error (SAErrorType::EXPR_VAR_NOT_ARRAY, expr.token_position,
                       (std::vector<std::shared_ptr<Type>>){ out_type });
            return nullptr;
          }

        if (out_type->is_array ())
          {
            out_type->array_sizes.erase (out_type->array_sizes.begin ());
          }
        else if (out_type->is_explicit_pointer ())
          {
            out_type->pointer_indirection -= 1;
          }

        return out_type;
      }
      break;

    case ASTType::EXPR_OPERAND_ACCESS_MEMBER:
      TODO ("Type of member");

    case ASTType::EXPR_OPERAND_CAST_TO:
      {
        auto cast_to_type = get_type_from_type_ast (expr.children.at (0));
        if (cast_to_type == nullptr)
          return nullptr;

        auto expr_type = get_type_from_expr_ast (expr.children.at (1));
        if (expr_type == nullptr)
          return nullptr;

        if (!can_convert_types (expr_type, cast_to_type))
          {
            add_error (SAErrorType::CANNOT_CAST_IN_CAST, expr.token_position,
                       (std::vector<std::shared_ptr<Type>>){ expr_type,
                                                             cast_to_type });
            return nullptr;
          }

        return cast_to_type->copy ();
      }

    case ASTType::EXPR_OPERAND_CALL:
      TODO ("Type of call");

    case ASTType::EXPR:
      {
        if (expr.children.size () > 1)
          {
            auto lhs_type = get_type_from_expr_ast (expr.children.at (0));
            if (lhs_type == nullptr)
              return nullptr;

            auto rhs_type = get_type_from_expr_ast (expr.children.at (2));
            if (rhs_type == nullptr)
              return nullptr;

            if (!can_convert_types (rhs_type, lhs_type))
              {
                add_error (SAErrorType::CANNOT_CONVERT_TYPES_EXPR,
                           expr.token_position);
                return nullptr;
              }

            const auto &binary_operator = expr.children.at (1);
            switch (binary_operator.type)
              {
              case ASTType::EXPR_BINARY_OPERATOR_XOR:
              case ASTType::EXPR_BINARY_OPERATOR_SHL:
              case ASTType::EXPR_BINARY_OPERATOR_SHR:
              case ASTType::EXPR_BINARY_OPERATOR_OR:
              case ASTType::EXPR_BINARY_OPERATOR_AND:
                {
                  std::shared_ptr<Type> floating_type
                      = lhs_type->is_floating ()
                            ? lhs_type
                            : (rhs_type->is_floating () ? rhs_type : nullptr);
                  if (floating_type != nullptr)
                    {
                      add_error (SAErrorType::BITWISE_OPERATION_ON_FLOAT,
                                 binary_operator.token_position);
                      return nullptr;
                    }
                }
                break;

              default:
                break;
              }

            return get_converted_type (rhs_type, lhs_type);
          }

        return get_type_from_expr_ast (expr.children.at (0));
      }

    case ASTType::PREFIX_EXPR:
      {
        auto operand = expr.children.at (0);
        auto operand_type = get_type_from_expr_ast (operand);

        for (size_t i = 1; i < expr.children.size (); i++)
          {
            const auto &prefix_operator = expr.children.at (i);
            switch (prefix_operator.type)
              {
              case ASTType::EXPR_PREFIX_OPERATOR_BNOT:
                {
                  if (operand_type->is_floating ())
                    {
                      add_error (SAErrorType::BITWISE_OPERATION_ON_FLOAT,
                                 prefix_operator.token_position);
                      return nullptr;
                    }
                }
                break;

              case ASTType::EXPR_PREFIX_OPERATOR_ADDRESS:
                {
                  switch (operand.type)
                    {
                    case ASTType::EXPR_OPERAND_NUM:
                    case ASTType::EXPR_OPERAND_NUMFLOAT:
                      add_error (SAErrorType::TAKE_ADDRESS_OF_NUMERIC_LITERAL,
                                 prefix_operator.token_position);
                      return nullptr;

                    case ASTType::EXPR_OPERAND_IDENTIFIER:
                      {
                        if (operand.value == "true"
                            || operand.value == "false")
                          {
                            add_error (
                                SAErrorType::TAKE_ADDRESS_OF_BOOLEAN_LITERAL,
                                prefix_operator.token_position);
                            return nullptr;
                          }
                        else if (operand.value == "nullptr")
                          {
                            add_error (
                                SAErrorType::TAKE_ADDRESS_OF_NULLPTR_LITERAL,
                                prefix_operator.token_position);
                            return nullptr;
                          }
                        // TODO: also check for enum elements
                      }
                      break;
                    default:
                      break;
                    }

                  operand_type->pointer_indirection += 1;
                  if (!operand_type->array_sizes.empty ())
                    {
                      operand_type->pointer_indirection
                          += operand_type->array_sizes.size ();
                      operand_type->array_sizes.clear ();
                    }
                }
                break;

              case ASTType::EXPR_PREFIX_OPERATOR_DEREF:
                {
                  switch (operand.type)
                    {
                    case ASTType::EXPR_OPERAND_NUM:
                    case ASTType::EXPR_OPERAND_NUMFLOAT:
                      add_error (SAErrorType::DEREF_ON_NUMERIC_LITERAL,
                                 prefix_operator.token_position);
                      return nullptr;

                    case ASTType::EXPR_OPERAND_IDENTIFIER:
                      {
                        if (operand.value == "true"
                            || operand.value == "false")
                          {
                            add_error (SAErrorType::DEREF_ON_BOOLEAN_LITERAL,
                                       prefix_operator.token_position);
                            return nullptr;
                          }
                        else if (operand.value == "nullptr")
                          {
                            add_error (SAErrorType::DEREF_ON_NULLPTR_LITERAL,
                                       prefix_operator.token_position);
                            return nullptr;
                          }
                        // TODO: also check for enum elements
                      }
                      break;
                    default:
                      break;
                    }

                  if (operand_type->pointer_indirection <= 0
                      || !operand_type->array_sizes.empty ())
                    {
                      add_error (SAErrorType::DEREF_ON_NONPTR,
                                 prefix_operator.token_position);
                      return nullptr;
                    }
                  operand_type->pointer_indirection -= 1;
                }
                break;

              default:
                break;
              }
          }

        return operand_type;
      }

    case ASTType::NAMESPACE:
      TODO ("Type from namespace");

    default:
      break;
    }

  return nullptr;
}

}
