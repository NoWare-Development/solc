#include "libsolc/util.hpp"
#include "sa/sa.hpp"
#include "types.hpp"

namespace solc
{

SemanticAnalyzer::SAError
SemanticAnalyzer::is_expr_known_at_comptime (const AST &expr) const
{
  switch (expr.type)
    {
    case ASTType::EXPR_OPERAND_NUM:
    case ASTType::EXPR_OPERAND_NUMFLOAT:
      return {};

    case ASTType::EXPR_OPERAND_IDENTIFIER:
      {
        return is_valid_literal (expr.value)
                   ? SAError{}
                   : SAError (SAErrorType::ID_IN_CONST_EXPR,
                              expr.token_position);
      }
      break;

    case ASTType::EXPR_OPERAND_SYMBOL:
      return {};

    case ASTType::EXPR_OPERAND_STRING:
      return { SAErrorType::STRING_IN_CONST_EXPR, expr.token_position };

    case ASTType::NAMESPACE:
      TODO ("Values from namespaces");

    case ASTType::EXPR_OPERAND_CAST_TO:
      return is_expr_known_at_comptime (expr.children.at (1));

    case ASTType::EXPR_OPERAND_CALL:
      return { SAErrorType::CALL_IN_CONST_EXPR, expr.token_position };

    case ASTType::EXPR_OPERAND_ARRAY_ELEMENT:
      return { SAErrorType::ARRAY_ELEM_IN_CONST_EXPR, expr.token_position };

    case ASTType::EXPR_OPERAND_ACCESS_MEMBER:
      return { SAErrorType::ACCESS_MEMBER_IN_CONST_EXPR, expr.token_position };

    case ASTType::EXPR:
      {
        auto lhs = is_expr_known_at_comptime (expr.children.at (0));
        if (lhs.type != SAErrorType::NONE)
          return lhs;

        auto rhs = is_expr_known_at_comptime (expr.children.at (2));
        if (rhs.type != SAErrorType::NONE)
          return rhs;

        return {};
      }

    case ASTType::PREFIX_EXPR:
      {
        auto operand_known = is_expr_known_at_comptime (expr.children.at (0));
        if (operand_known.type != SAErrorType::NONE)
          return operand_known;

        const auto expr_len = expr.children.size ();
        for (size_t i = 1; i < expr_len; i++)
          {
            const auto &prefix = expr.children.at (i);
            switch (prefix.type)
              {
              case ASTType::EXPR_PREFIX_OPERATOR_BNOT:
              case ASTType::EXPR_PREFIX_OPERATOR_NEG:
              case ASTType::EXPR_PREFIX_OPERATOR_NOT:
                continue;

              case ASTType::EXPR_PREFIX_OPERATOR_ADDRESS:
                return { SAErrorType::TAKE_ADDRESS_IN_CONST_EXPR,
                         prefix.token_position };

              case ASTType::EXPR_PREFIX_OPERATOR_DEREF:
                return { SAErrorType::DEREF_IN_CONST_EXPR,
                         prefix.token_position };

              default:
                NOREACH ();
              }
          }

        return {};
      }

    default:
      NOREACH ();
    }
}

std::shared_ptr<Type>
SemanticAnalyzer::get_type_from_expr_ast (const AST &expr, bool taking_address)
{
  switch (expr.type)
    {
    case ASTType::EXPR_OPERAND_NUM:
    case ASTType::EXPR_OPERAND_NUMFLOAT:
      {
        if (taking_address)
          {
            add_error (SAErrorType::TAKE_ADDRESS_OF_A_LITERAL,
                       expr.token_position);
            return nullptr;
          }

        if (!expr.children.empty ())
          {
            const auto &typespec = expr.children.at (0);
            if (!is_valid_typespec (typespec.value))
              {
                add_error (SAErrorType::INVALID_TYPESPEC,
                           typespec.token_position);
                return nullptr;
              }

            auto typespec_type = get_typespec (typespec.value)->copy ();
            if (!typespec_type->is_floating ()
                && expr.type == ASTType::EXPR_OPERAND_NUMFLOAT)
              {
                add_error (SAErrorType::INT_TYPESPEC_ON_FLOAT,
                           typespec.token_position);
                return nullptr;
              }

            return typespec_type;
          }

        return expr.type == ASTType::EXPR_OPERAND_NUM
                   ? _basic_types.at ("int")->copy ()
                   : _basic_types.at ("double")->copy ();
      }
      break;

    case ASTType::EXPR_OPERAND_IDENTIFIER:
      {
        if (is_valid_literal (expr.value))
          {
            if (taking_address)
              {
                add_error (SAErrorType::TAKE_ADDRESS_OF_A_LITERAL,
                           expr.token_position);
                return nullptr;
              }
            return get_literal_type (expr.value);
          }

        TODO ("Non-literal identifiers");
      }

    case ASTType::EXPR_OPERAND_SYMBOL:
      {
        if (taking_address)
          {
            add_error (SAErrorType::TAKE_ADDRESS_OF_A_LITERAL,
                       expr.token_position);
            return nullptr;
          }

        return get_basic_type ("char")->copy ();
      }

    case ASTType::EXPR_OPERAND_STRING:
      {
        auto char_type = get_basic_type ("char")->copy ();
        char_type->pointer_indirection += 1;
        return char_type;
      }

    case ASTType::NAMESPACE:
      {
        TODO ("Namespaced identifiers (consts, calls, enum elements)");
      }

    case ASTType::EXPR_OPERAND_CALL:
      {
        TODO ("Calls");
      }

    case ASTType::EXPR_OPERAND_ARRAY_ELEMENT:
      {
        TODO ("Array elements");
      }

    case ASTType::EXPR_OPERAND_ACCESS_MEMBER:
      {
        TODO ("Access member");
      }

    case ASTType::EXPR_OPERAND_CAST_TO:
      {
        auto cast_type = get_type_from_type_ast (expr.children.at (0));
        if (cast_type == nullptr)
          return nullptr;

        auto expr_type
            = get_type_from_expr_ast (expr.children.at (1), taking_address);
        if (expr_type == nullptr)
          return nullptr;

        if (can_convert_types (expr_type, cast_type))
          return cast_type;

        add_error (
            SAErrorType::CANNOT_CAST, expr.token_position,
            (std::vector<std::shared_ptr<Type>>){ expr_type, cast_type });
        return nullptr;
      }

    case ASTType::EXPR:
      {
        const auto &lhs = expr.children.at (0);
        const auto &rhs = expr.children.at (2);

        auto lhs_type = get_type_from_expr_ast (lhs, taking_address);
        if (lhs_type == nullptr)
          return nullptr;

        auto rhs_type = get_type_from_expr_ast (rhs, taking_address);
        if (rhs_type == nullptr)
          return nullptr;

        const auto &op = expr.children.at (1);

        if (rhs_type->is_floating () || lhs_type->is_floating ())
          {
            switch (op.type)
              {
              case ASTType::EXPR_BINARY_OPERATOR_AND:
              case ASTType::EXPR_BINARY_OPERATOR_OR:
              case ASTType::EXPR_BINARY_OPERATOR_SHL:
              case ASTType::EXPR_BINARY_OPERATOR_SHR:
              case ASTType::EXPR_BINARY_OPERATOR_XOR:
                {
                  auto floating_position = rhs_type->is_floating ()
                                               ? rhs.token_position
                                               : lhs.token_position;

                  add_error (SAErrorType::BITWISE_OP_ON_FLOAT,
                             (std::vector<size_t>){ op.token_position,
                                                    floating_position });
                  return nullptr;
                }

              default:
                break;
              }
          }

        if (rhs_type->is_complex () || rhs_type->is_pointer ())
          {
            add_error (SAErrorType::BINOP_ON_NONNUM, op.token_position,
                       (std::vector<std::shared_ptr<Type>>){ rhs_type });
            return nullptr;
          }
        else if (lhs_type->is_complex () || lhs_type->is_pointer ())
          {
            add_error (SAErrorType::BINOP_ON_NONNUM, op.token_position,
                       (std::vector<std::shared_ptr<Type>>){ lhs_type });
            return nullptr;
          }

        if (!can_convert_types (rhs_type, lhs_type))
          {
            add_error (
                SAErrorType::CANNOT_CONVERT_TYPES_IN_EXPR,
                expr.children.at (2).token_position,
                (std::vector<std::shared_ptr<Type>>){ rhs_type, lhs_type });
            return nullptr;
          }

        return get_converted_type (rhs_type, lhs_type);
      }

    case ASTType::PREFIX_EXPR:
      {
        const auto expr_len = expr.children.size ();

        // Should probably be done in other way but it was the easiest
        // implementation I could came up with
        for (size_t i = 1; i < expr_len; i++)
          {
            if (expr.children.at (i).type
                == ASTType::EXPR_PREFIX_OPERATOR_ADDRESS)
              {
                taking_address = true;
                break;
              }
          }

        auto operand_type
            = get_type_from_expr_ast (expr.children.at (0), taking_address);
        if (operand_type == nullptr)
          return nullptr;

        for (size_t i = 1; i < expr_len; i++)
          {
            const auto &prefix_operand = expr.children.at (i);
            switch (prefix_operand.type)
              {
              case ASTType::EXPR_PREFIX_OPERATOR_DEREF:
                {
                  if (!operand_type->is_explicit_pointer ())
                    {
                      add_error (SAErrorType::DEREF_ON_NONPTR,
                                 prefix_operand.token_position,
                                 (std::vector<std::shared_ptr<Type>>){
                                     operand_type });
                      return nullptr;
                    }

                  operand_type->pointer_indirection--;
                }

              case ASTType::EXPR_PREFIX_OPERATOR_ADDRESS:
                {
                  if (operand_type->get_full_pointer_indirection () >= 127)
                    {
                      add_error (SAErrorType::TOO_BIG_PTR_INDIRECTION,
                                 prefix_operand.token_position);
                      return nullptr;
                    }
                  operand_type->pointer_indirection++;
                }
                break;

              case ASTType::EXPR_PREFIX_OPERATOR_NEG:
                {
                  if (!operand_type->is_integer ()
                      && !operand_type->is_floating ())
                    {
                      add_error (SAErrorType::NEG_ON_NONNUM,
                                 prefix_operand.token_position,
                                 (std::vector<std::shared_ptr<Type>>){
                                     operand_type });
                      return nullptr;
                    }
                }
                break;

              default:
                break;
              }
          }

        return operand_type;
      }

    default:
      NOREACH ();
    }
}

size_t
SemanticAnalyzer::get_expr_comptime_ivalue (const AST &expr) const
{
  switch (expr.type)
    {
    case ASTType::EXPR_OPERAND_NUM:
      {
        return util::string_to_int (expr.value);
      }
    case ASTType::EXPR_OPERAND_NUMFLOAT:
      {
        return (size_t)util::string_to_double (expr.value);
      }

    case ASTType::EXPR_OPERAND_SYMBOL:
      {
        if (expr.value.at (0) == '\\')
          {
            switch (expr.value.at (1))
              {
              case '\\':
                return '\\';

              case 'n':
                return '\n';

              case 't':
                return '\t';

              case 'r':
                return '\r';

              default:
                NOREACH ();
              }
          }
        return (size_t)expr.value.at (0);
      }

    case ASTType::EXPR_OPERAND_IDENTIFIER:
      {
        if (expr.value == "nullptr" || expr.value == "false")
          {
            return 0;
          }
        else if (expr.value == "true")
          {
            return 1;
          }
      }

    case ASTType::NAMESPACE:
      {
        TODO ("Constant values from namespaces (enum fields)");
      }

    case ASTType::EXPR:
      {
        auto lhs = get_expr_comptime_ivalue (expr.children.at (0));
        auto rhs = get_expr_comptime_ivalue (expr.children.at (2));

        switch (expr.children.at (1).type)
          {
          case ASTType::EXPR_BINARY_OPERATOR_ADD:
            return lhs + rhs;
          case ASTType::EXPR_BINARY_OPERATOR_AND:
            return lhs & rhs;
          case ASTType::EXPR_BINARY_OPERATOR_DIV:
            return lhs / rhs;
          case ASTType::EXPR_BINARY_OPERATOR_MOD:
            return lhs % rhs;
          case ASTType::EXPR_BINARY_OPERATOR_MUL:
            return lhs * rhs;
          case ASTType::EXPR_BINARY_OPERATOR_OR:
            return lhs | rhs;
          case ASTType::EXPR_BINARY_OPERATOR_SHL:
            return lhs << rhs;
          case ASTType::EXPR_BINARY_OPERATOR_SHR:
            return lhs >> rhs;
          case ASTType::EXPR_BINARY_OPERATOR_SUB:
            return lhs - rhs;
          case ASTType::EXPR_BINARY_OPERATOR_XOR:
            return lhs ^ rhs;
          default:
            NOREACH ();
          }
      }

    case ASTType::PREFIX_EXPR:
      {
        auto operand = get_expr_comptime_ivalue (expr.children.at (0));

        const auto expr_len = expr.children.size ();
        for (size_t i = 1; i < expr_len; i++)
          {
            const auto &prefix_op = expr.children.at (i);
            switch (prefix_op.type)
              {
              case ASTType::EXPR_PREFIX_OPERATOR_BNOT:
                operand = ~operand;
                break;
              case ASTType::EXPR_PREFIX_OPERATOR_NEG:
                operand = -operand;
                break;
              case ASTType::EXPR_PREFIX_OPERATOR_NOT:
                operand = !operand;
                break;

              default:
                NOREACH ();
              }
          }

        return operand;
      }

    default:
      break;
    }

  NOREACH ();
}

}
