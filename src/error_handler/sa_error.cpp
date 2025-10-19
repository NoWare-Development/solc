#include "error_handler.hpp"
#include "util/util.hpp"
#include <sa/sa.hpp>
#include <types.hpp>

using SAErrorType = solc::SemanticAnalyzer::SAErrorType;

std::string ErrorHandler::get_sa_error_reason(
  const solc::SemanticAnalyzer::SAError &err) const
{
  std::string out{};
  switch (err.type) {
  case SAErrorType::NONE:
    out += "solc::SemanticAnalyzer::SAErrorType::NONE -- This message "
           "should "
           "never occur";
    break;

  case SAErrorType::UNDEFINED_TYPE:
    out += "unknown type \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\"";
    break;

  case SAErrorType::SYMBOL_REDEF:
    out += "redefinition of symbol \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\"";
    break;

  case SAErrorType::TYPE_REDEF:
    out += "redefinition of type \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\"";
    break;

  case SAErrorType::VAR_REDECL:
    out += "redeclaration of variable \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\"";
    break;

  case SAErrorType::ARRAY_IN_TYPEDEF:
    out += "array declaration in type definition";
    break;

  case SAErrorType::INVALID_TYPESPEC:
    out += "invalid numeric literal type specifier \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\"";
    break;

  case SAErrorType::INT_TYPESPEC_ON_FLOAT:
    out += "integer type specifier \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\" applied to floating-point numeric literal";
    break;

  case SAErrorType::ID_IN_CONST_EXPR:
    out += "variables in constant expressions are not allowed";
    break;

  case SAErrorType::STRING_IN_CONST_EXPR:
    out += "strings in constant expressions are not allowed";
    break;

  case SAErrorType::CALL_IN_CONST_EXPR:
    out += "calls in constant expressions are not allowed";
    break;

  case SAErrorType::ARRAY_ELEM_IN_CONST_EXPR:
    out += "array elements in constant expressions are not allowed";
    break;

  case SAErrorType::ACCESS_MEMBER_IN_CONST_EXPR:
    out += "accessing members of structs or unions in constant expressions "
           "is not allowed";
    break;

  case SAErrorType::TAKE_ADDRESS_IN_CONST_EXPR:
    out += "taking address of a value in constant expressions is not "
           "allowed";
    break;

  case SAErrorType::DEREF_IN_CONST_EXPR:
    out += "dereferncing value in constant expressions is not allowed";
    break;

  case SAErrorType::CANNOT_CAST:
    out += "cannot cast value of type ";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += err.types.at(0)->to_string();
    out += escape_reset();
    out += " to type ";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += err.types.at(1)->to_string();
    out += escape_reset();
    break;

  case SAErrorType::CANNOT_CONVERT_TYPES_IN_EXPR:
    out += "cannot convert value of type ";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += err.types.at(0)->to_string();
    out += escape_reset();
    out += " to type ";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += err.types.at(1)->to_string();
    out += escape_reset();
    break;

  case SAErrorType::BITWISE_OP_ON_FLOAT:
    out += "bitwise operation on a floating-point value \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(1)).value;
    out += escape_reset();
    out += "\"";
    break;

  case SAErrorType::TAKE_ADDRESS_OF_A_LITERAL:
    out += "taking address of a literal \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\"";
    break;

  case SAErrorType::DEREF_ON_NONPTR:
    out += "dereferencing value of type ";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += err.types.at(0)->to_string();
    out += escape_reset();
    out += " which is not a pointer";
    break;

  case SAErrorType::TOO_BIG_PTR_INDIRECTION:
    out += "taking address exceeds allowed pointer limits";
    break;

  case SAErrorType::NEG_ON_NONNUM:
    out += "trying to get a negative of value of type ";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += err.types.at(0)->to_string();
    out += escape_reset();
    out += " which is not a number";
    break;

  case SAErrorType::BINOP_ON_NONNUM:
    out += "trying to apply binary operation \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\" on a value of type ";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += err.types.at(0)->to_string();
    out += escape_reset();
    out += " which is not a number";
    break;

  case SAErrorType::NONINT_IN_ARRAY_DECL:
    out += "in array declaration value \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\" of type ";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += err.types.at(0)->to_string();
    out += escape_reset();
    out += " is not an integer";
    break;

  case SAErrorType::ENUM_IS_EMPTY:
    out += "enum \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\" is empty";
    break;

  case SAErrorType::ENUM_ELEM_EXPR_NOT_INT:
    out += "enum element \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\" is assinged to not an integer \"";
    break;

  case SAErrorType::ENUM_ELEM_REDEF:
    out += "redefinition of enum element \"";
    out += escape_graphics(ESCGraphics::ESCGRAPHICS_BOLD);
    out += _tokens.at(err.token_positions.at(0)).value;
    out += escape_reset();
    out += "\"";
    break;

  default:
    out = "<Unknown semantic analyzer error>";
    break;
  }

  return out;
}
