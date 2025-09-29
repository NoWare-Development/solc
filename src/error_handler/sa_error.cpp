#include "error_handler.hpp"
#include "util/util.hpp"
#include <sa/sa.hpp>
#include <types.hpp>

using SAErrorType = nlc::SemanticAnalyzer::SAErrorType;

std::string
ErrorHandler::get_sa_error_reason (
    const nlc::SemanticAnalyzer::SAError &err) const
{
  std::string out{};
  switch (err.type)
    {
    case SAErrorType::NONE:
      out += "nlc::SemanticAnalyzer::SAErrorType::NONE -- This message should "
             "never occur";
      break;

    case SAErrorType::UNDEFINED_TYPE:
      out += "unknown type \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += _tokens.at (err.token_positions.at (0)).value;
      out += escape_reset ();
      out += "\"";
      break;

    case SAErrorType::TYPE_REDEF:
      out += "redefinition of type \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += _tokens.at (err.token_positions.at (0)).value;
      out += escape_reset ();
      out += "\"";
      break;

    case SAErrorType::VAR_REDECL:
      out += "redeclaration of variable \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += _tokens.at (err.token_positions.at (0)).value;
      out += escape_reset ();
      out += "\"";
      break;

    case SAErrorType::ARRAY_IN_TYPEDEF:
      out += "arrays in type definitions are not allowed";
      break;

    case SAErrorType::ARRAY_SIZE_NOT_COMPTIME:
      out += "array size is not constant";
      break;

    case SAErrorType::ARRAY_SIZE_NOT_INTEGER:
      out += "array size is not an integer";
      break;

    case SAErrorType::IDENTIFIER_IN_CONST_EXPR:
      out += "variable in constant expression";
      break;

    case SAErrorType::DEREF_IN_CONST_EXPR:
      out += "dereference in constant expression";
      break;

    case SAErrorType::ADDRESS_IN_CONST_EXPR:
      out += "taking address of a value in constant expression";
      break;

    case SAErrorType::ARRAY_ELEM_IN_CONST_EXPR:
      out += "value of array element in constant expression";
      break;

    case SAErrorType::ACCESS_MEMBER_IN_CONST_EXPR:
      out += "accessing member of struct/union in constant expression";
      break;

    case SAErrorType::CALL_IN_CONST_EXPR:
      out += "call to function in constant expression";
      break;

    case SAErrorType::STRING_IN_CONST_EXPR:
      out += "string literal in constant expression";
      break;

    case SAErrorType::CANNOT_CONVERT_TYPES_EXPR:
      // TODO: expand error
      out += "cannot convert types in expression";
      break;

    case SAErrorType::BITWISE_OPERATION_ON_FLOAT:
      out += "bitwise operation on a floating-point value";
      break;

    case SAErrorType::UNKNOWN_NUMERIC_TYPESPEC:
      out += "unknown numeric type specifier \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += _tokens.at (err.token_positions.at (0)).value;
      out += escape_reset ();
      out += "\"";
      break;

    case SAErrorType::INT_TYPESPEC_ON_FLOAT:
      out += "integer type specifier \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += _tokens.at (err.token_positions.at (0)).value;
      out += escape_reset ();
      out += "\" is applied to floating-point value";
      break;

    case SAErrorType::TAKE_ADDRESS_OF_NUMERIC_LITERAL:
      out += "attempting to take address of numeric literal";
      break;

    case SAErrorType::TAKE_ADDRESS_OF_BOOLEAN_LITERAL:
      out += "attempting to take address of boolean literal";
      break;

    case SAErrorType::TAKE_ADDRESS_OF_NULLPTR_LITERAL:
      out += "attempting to take address of `nullptr'";
      break;

    case SAErrorType::TAKE_ADDRESS_OF_ENUM_ELEMENT:
      out += "attempting to take address of enum element";
      break;

    case SAErrorType::DEREF_ON_NUMERIC_LITERAL:
      out += "attempting to dereference numeric literal";
      break;

    case SAErrorType::DEREF_ON_BOOLEAN_LITERAL:
      out += "attempting to dereference boolean literal";
      break;

    case SAErrorType::DEREF_ON_NULLPTR_LITERAL:
      out += "attempting to dereference `nullptr'";
      break;

    case SAErrorType::DEREF_ON_ENUM_ELEMENT:
      out += "attempting to dereference enum element";
      break;

    case SAErrorType::EXPR_VAR_NOT_ARRAY:
      out += "variable \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += _tokens.at (err.token_positions.at (0)).value;
      out += escape_reset ();
      out += "\" of type \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += err.types.at (0)->to_string ();
      out += escape_reset ();
      out += "\"is not an array or a pointer";
      break;

    case SAErrorType::CANNOT_CAST_IN_CAST:
      out += "cannot cast value of type \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += err.types.at (0)->to_string ();
      out += escape_reset ();
      out += "to type \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += err.types.at (1)->to_string ();
      out += escape_reset ();
      out += "\"";
      break;

    default:
      out = "<Unknown semantic analyzer error>";
      break;
    }

  return out;
}
