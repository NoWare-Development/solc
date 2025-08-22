#include "error_handler.hpp"
#include "../util/util.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include "semantic-analyzer/sa.hpp"
#include "semantic-analyzer/types.hpp"
#include <iostream>
#include <string>

ErrorHandler::ErrorHandler (const std::string &filename,
                            const std::string &src)
{
  _filename = filename;
  _src = src;

  for (auto &c : _src)
    {
      if (c == '\n')
        {
          _nol++;
        }
    }

  _numoffset = get_number_length (_nol);
}

void
ErrorHandler::add_tokens (const std::vector<nlc::Token> &tokens)
{
  _tokens = tokens;
}

void
ErrorHandler::add_parser_errors (
    const std::vector<nlc::Parser::ParserError> &errors)
{
  _parser_errors = errors;
}

bool
ErrorHandler::handle_tokens () const
{
  if (has_invalid_tokens ())
    {
      print_errored_tokens ();
      return false;
    }
  return true;
}

bool
ErrorHandler::handle_parser_errors () const
{
  if (has_parser_errors ())
    {
      print_parser_errors ();
      return false;
    }

  return true;
}

bool
ErrorHandler::handle_invalid_expressions (const nlc::AST &root) const
{
  std::vector<nlc::AST> errored_asts{};
  collect_invalid_expressions (root, errored_asts);

  if (!errored_asts.empty ())
    {
      for (const auto &e : errored_asts)
        {
          if (e.token_position >= _tokens.size ())
            continue;

          const auto &token = _tokens.at (e.token_position);

          std::string message = get_message_start (
              token.line + 1, token.end - token.len + 1, "error",
              ESCColor::ESCCOLOR_RED, ESCGraphics::ESCGRAPHICS_BOLD);

          message += "Expression error: ";
          message += e.value;
          message += '\n';

          message += get_highlighted_token (token, ESCColor::ESCCOLOR_RED,
                                            ESCGraphics::ESCGRAPHICS_BOLD);
          message += '\n';

          std::cout << message;
        }
      return false;
    }

  return true;
}

void
ErrorHandler::handle_sa_errors (
    const std::vector<nlc::sa::SemanticAnalyzer::SAError> &errors) const
{
  for (const auto &e : errors)
    {
      std::string err_reason = get_semantic_analyzer_error_reason (e);
      const nlc::Token &tok = _tokens.at (e.positions.at (0));

      std::string message = get_message_start (
          tok.line + 1, tok.end - tok.len + 1, "error", ESCColor::ESCCOLOR_RED,
          ESCGraphics::ESCGRAPHICS_BOLD);

      message += err_reason;
      message += '\n';

      message += get_highlighted_token (tok, ESCColor::ESCCOLOR_RED,
                                        ESCGraphics::ESCGRAPHICS_BOLD);

      std::cout << message << '\n';
    }
}

void
ErrorHandler::collect_invalid_expressions (const nlc::AST &node,
                                           std::vector<nlc::AST> &v) const
{
  for (const auto &c : node.children)
    {
      if (c.type == nlc::ASTType::AST_EXPR)
        {
          for (const auto &ec : c.children)
            {
              if (ec.type == nlc::ASTType::AST_ERR)
                {
                  v.push_back (ec);
                }
            }
        }

      collect_invalid_expressions (c, v);
    }
}

void
ErrorHandler::print_errored_tokens () const
{
  auto get_invalid_tokens
      = [] (const std::vector<nlc::Token> &tokens) -> std::vector<nlc::Token> {
    std::vector<nlc::Token> invalid_tokens{};

    for (auto &tok : tokens)
      {
        if (tok.type == nlc::TokenType::TOKEN_ERR)
          {
            invalid_tokens.push_back (tok);
          }
      }
    return invalid_tokens;
  };

  std::vector<nlc::Token> invalid_tokens = get_invalid_tokens (_tokens);

  for (auto &tok : invalid_tokens)
    {
      std::cout << get_token_error (tok) << '\n';
    }
}

void
ErrorHandler::print_parser_errors () const
{
  for (auto &err : _parser_errors)
    {
      std::cout << get_parser_error (err) << '\n';
    }
}

std::string
ErrorHandler::get_token_error (const nlc::Token &tok) const
{
  std::string error_string{};

  error_string += get_message_start (tok.line + 1, tok.end - tok.len + 1,
                                     "error", ESCColor::ESCCOLOR_RED,
                                     ESCGraphics::ESCGRAPHICS_BOLD);
  error_string += "Unidentified token '";
  error_string += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
  error_string += tok.value;
  error_string += escape_reset ();
  error_string += "'\n";

  error_string += get_highlighted_token (tok, ESCColor::ESCCOLOR_RED,
                                         ESCGraphics::ESCGRAPHICS_BOLD);

  return error_string;
}

std::string
ErrorHandler::get_parser_error (const nlc::Parser::ParserError &err) const
{
  const std::string error_reason = get_parser_error_reason (err);
  std::string error_string{};

  if (err.type != nlc::Parser::ParserError::ErrType::PARSER_ERROR_UNEXPECTED)
    {
      error_string += get_message_start (_nol, last_line ().length (), "error",
                                         ESCColor::ESCCOLOR_RED,
                                         ESCGraphics::ESCGRAPHICS_BOLD);
    }
  else
    {
      const auto &token = _tokens.at (err.pos);
      error_string += get_message_start (
          token.line + 1, token.end - token.len + 1, "error",
          ESCColor::ESCCOLOR_RED, ESCGraphics::ESCGRAPHICS_BOLD);
    }
  error_string += error_reason;

  if (err.type == nlc::Parser::ParserError::ErrType::PARSER_ERROR_UNEXPECTED)
    {
      error_string += '\n';
      error_string += get_highlighted_token (_tokens.at (err.pos),
                                             ESCColor::ESCCOLOR_RED,
                                             ESCGraphics::ESCGRAPHICS_BOLD);
    }

  return error_string;
}

std::string
ErrorHandler::get_message_start (size_t line, size_t start,
                                 const std::string &msg, ESCColor color,
                                 ESCGraphics mode) const
{
  std::string out{};

  out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
  out += _filename;
  out += ":" + std::to_string (line) + ":" + std::to_string (start);
  out += ": ";

  out += escape_graphics (mode);
  out += escape_color (color);
  out += msg + ": ";
  out += escape_reset ();

  return out;
}

std::string
ErrorHandler::get_highlighted_token (const nlc::Token &tok, ESCColor color,
                                     ESCGraphics mode) const
{
  constexpr const size_t base_offset = 2;
  constexpr const char *separator = " | ";

  std::string out{};

  const size_t line_num = tok.line + 1;
  const size_t line_num_len = get_number_length (line_num);

  size_t line_num_offset = _numoffset - line_num_len + base_offset;
  for (; line_num_offset > 0; line_num_offset--)
    {
      out += ' ';
    }

  out += std::to_string (line_num);
  out += separator;

  auto line = get_line (tok.line);
  line.insert (tok.end + 1, escape_reset ());
  line.insert (tok.end - tok.len + 1, escape_color (color));
  line.insert (tok.end - tok.len + 1, escape_graphics (mode));

  out += line;
  out += '\n';

  for (size_t i = 0; i < _numoffset + 2; i++)
    {
      out += ' ';
    }
  out += separator;

  const size_t mark_offset = tok.end - tok.len + 1;
  for (size_t i = 0; i < mark_offset; i++)
    {
      out += ' ';
    }

  out += escape_graphics (mode);
  out += escape_color (color);

  out += '^';
  for (size_t i = 0; i < tok.len - 1; i++)
    {
      out += '~';
    }

  out += escape_reset ();

  return out;
}

std::string
ErrorHandler::get_parser_error_reason (
    const nlc::Parser::ParserError &err) const
{
  using namespace nlc;

  switch (err.type)
    {
    case Parser::ParserError::ErrType::PARSER_ERROR_UNK:
      return "<Unknown error>";

    case Parser::ParserError::ErrType::PARSER_ERROR_EXPECTED:
      {
        std::string out{};
        out += "Expected token";
        return out;
      }

    case Parser::ParserError::ErrType::PARSER_ERROR_UNEXPECTED:
      {
        std::string out{};
        out += "Unexpected token \"" + _tokens.at (err.pos).value + "\"";
        return out;
      }

    case Parser::ParserError::ErrType::PARSER_ERROR_INVALID_EXPR:
      {
        std::string out{};
        out += "Invalid expression at line "
               + std::to_string (_tokens.at (err.pos).line + 1);
        return out;
      }

    default:
      return {};
    }
}

bool
ErrorHandler::has_invalid_tokens () const
{
  for (auto &tok : _tokens)
    {
      if (tok.type == nlc::TokenType::TOKEN_ERR)
        {
          return true;
        }
    }
  return false;
}

bool
ErrorHandler::has_parser_errors () const
{
  return _parser_errors.size () > 0;
}

std::string
ErrorHandler::get_line (size_t linenum) const
{
  size_t pos = 0;
  // Skip until found line by number
  while (pos < _src.length () && linenum > 0)
    {
      if (_src.at (pos++) == '\n')
        {
          linenum--;
        }
    }

  std::string buf{};
  while (pos < _src.length ())
    {
      char c = _src.at (pos++);
      if (c == '\n')
        {
          break;
        }
      buf += c;
    }
  return buf;
}

std::string
ErrorHandler::last_line () const
{
  return get_line (_nol - 1);
}

size_t
ErrorHandler::get_number_length (long long num) const
{
  size_t len = 0;
  if (num == 0)
    {
      return 1;
    }
  else if (num < 0)
    {
      len++;
      num *= -1;
    }

  while (num > 0)
    {
      num /= 10;
      len++;
    }

  return len;
}

std::string
ErrorHandler::get_semantic_analyzer_error_reason (
    const nlc::sa::SemanticAnalyzer::SAError &err) const
{
  using namespace nlc::sa;

  std::string out{};
  switch (err.type)
    {
    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_WRONG_AST_TYPE:
      {
        out = "Wrong AST type (compiler bug): \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_NO_NAME_FOR_VARIABLE:
      {
        out = "No name for variable (compiler bug): \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_WRONG_NUMBER_OF_ARRAYS_CHILDREN:
      {
        out = "Wrong number of array's children (compiler bug): \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_WRONG_NUMBER_OF_POINTERS_CHILDREN:
      {
        out = "Wrong number of pointer's children (compiler bug): \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_WRONG_NUMBER_OF_INTLIT_CHILDREN:
      {
        out = "Wrong number of integer literal's children (compiler bug): \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_WRONG_NUMBER_OF_FLOATLIT_CHILDREN:
      {
        out = "Wrong number of float literal's children (compiler bug): \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_WRONG_NUMBER_OF_EXPR_CHILDREN:
      {
        out = "Wrong number of expression's children (compiler bug): \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_TYPE_HAS_NO_PLAIN_TYPE:
      {
        out = "Type has no plain type (compiler bug): \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_TYPE_IS_NOT_DEFINED:
      {
        out = "Type \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" is not defined";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_INVALID_TYPESPEC:
      {
        out = "Type specifier \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" is not valid";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_CANNOT_CONVERT_TYPES_IN_EXPR:
      {
        out = "Cannot convert type of \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" to type of \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (1)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_CANNOT_CONVERT_TYPES_IN_VARDEF:
      {
        out = "Cannot convert expression to type of \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (1)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_CANNOT_CONVERT_TYPES_IN_CAST:
      {
        out = "Cannot cast type of \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_CANNOT_CONVERT_FLOAT_TO_INTTYPE_TYPESPEC:
      {
        out = "Cannot convert floating-point value to an integer with type "
              "specifier \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\"";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_CANNOT_CONVERT_FUNCTION_ARGUMENT:
      {
        out = "Cannot convert argument for function \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" ";

        out += "from \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += get_type_name_from_sa_type (err.types.at (0));
        out += escape_reset ();
        out += "\" ";

        out += "to \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += get_type_name_from_sa_type (err.types.at (1));
        out += escape_reset ();
        out += "\"";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_UNDECLARED_VARIABLE:
      {
        out = "Variable \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" is not declared";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_UNDECLARED_FUNCTION:
      {
        out = "Function \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" is not declared";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_DEREFERENCING_NONPOINTER_VALUE:
      {
        out = "Attempt to dereference a non-pointer value";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_BINARY_NOT_ON_NON_INTEGER:
      {
        out = "Binary not on a non-integer value";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_BINARY_NOT_ON_A_POINTER:
      {
        out = "Binary not on a pointer value";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_NEGATIVE_OF_VOID:
      {
        out = "Taking negative of a value of type \"void\"";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_NEGATIVE_OF_POINTER:
      {
        out = "Taking negative of a pointer";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_NEGATIVE_OF_STRUCT:
      {
        out = "Taking negative of a structure";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_NEGATIVE_OF_UNION:
      {
        out = "Taking negative of a union";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_VARIABLE_ALREADY_DECLARED:
      {
        out = "Variable \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" is already declared";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_FUNCTION_ALREADY_DECLARED:
      {
        out = "Function \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" is already declared";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_STRUCTURE_ALREADY_DEFINED:
      {
        out = "Structure \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" is already defined";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_UNION_ALREADY_DEFINED:
      {
        out = "Union \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" is already defined";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_ENUM_ALREADY_DEFINED:
      {
        out = "Enum \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" is already defined";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_SIZE_IS_NOT_KNOWN_AT_COMPTIME:
      {
        out = "Size of variable \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" is not known at compile time";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::SA_ERR_TYPE_ARRAY_NOT_RAW_INT:
      {
        out = "Array size is not an integer value: ";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_ARRAY_PREFIXOP_IS_NOT_COMPTIME:
      {
        out = "Prefix operator \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
        out += "\" cannot be used in array size expression because the result "
               "is not known at compile time";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_ARRAY_EXPR_IS_NOT_COMPTIME:
      {
        out = "Expression result is not known at compile time: ";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_EXPR_CANNOT_DEREFERENCE_INTLIT:
      {
        out = "Cannot dereference integer value";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_EXPR_CANNOT_DEREFERENCE_FLOATLIT:
      {
        out = "Cannot dereference floating-point value";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_EXPR_CANNOT_DEREFERENCE_BOOLLIT:
      {
        out = "Cannot dereference boolean value";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_EXPR_CANNOT_DEREFERENCE_ENUMLIT:
      {
        out = "Cannot dereference enum member value";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_EXPR_CANNOT_TAKE_ADDRESS_OF_INTLIT:
      {
        out = "Cannot take address of integer value";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_EXPR_CANNOT_TAKE_ADDRESS_OF_FLOATLIT:
      {
        out = "Cannot take address of floating-point value";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_EXPR_CANNOT_TAKE_ADDRESS_OF_BOOLLIT:
      {
        out = "Cannot take address of boolean value";
      }
      break;
    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_EXPR_CANNOT_TAKE_ADDRESS_OF_ENUMLIT:
      {
        out = "Cannot take address of enum member value";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_CANNOT_CAST_VALUE_TO_AN_ARRAY:
      {
        out = "Cannot cast value to an array";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_VARDEFS_IN_FUNCPTR_ARGS_ARE_NOT_ALLOWED:
      {
        out = "Variable definitions in function pointer declarations are not "
              "allowed";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_CANNOT_ASSIGN_NONPOINTER_VALUE_TO_A_POINTER:
      {
        out = "Cannot assign non-pointer value to a variable of pointer type";
      }
      break;

    case SemanticAnalyzer::SAError::ErrType::
        SA_ERR_TYPE_INVALID_NUMBER_OF_ARGUMENTS_IN_FUNCTION:
      {
        out = "Invalid number of arguments for function \"";
        out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
        out += _tokens.at (err.positions.at (0)).value;
        out += escape_reset ();

        out += "\", expected: ";
        out += err.expected;

        out += " , got: ";
        out += err.got;
      }
      break;

    default:
      out = "<Unknown error>";
      break;
    }

  return out;
}

std::string
ErrorHandler::get_type_name_from_sa_type (const nlc::sa::Type &type) const
{
  std::string base_type{};
  std::string pointer_str{};

  for (size_t i = 0; i < type.pointer_count; i++)
    {
      pointer_str += '*';
    }

  switch (type.type)
    {
    case nlc::sa::BuiltinType::BUILTIN_TYPE_UNK:
      base_type = "<UNKNOWN TYPE>";
      break;

    case nlc::sa::BuiltinType::BUILTIN_TYPE_VOID:
      base_type = "void";
      break;

    case nlc::sa::BuiltinType::BUILTIN_TYPE_UCHAR:
      base_type = "uchar";
      break;
    case nlc::sa::BuiltinType::BUILTIN_TYPE_USHORT:
      base_type = "ushort";
      break;
    case nlc::sa::BuiltinType::BUILTIN_TYPE_UINT:
      base_type = "uint";
      break;
    case nlc::sa::BuiltinType::BUILTIN_TYPE_ULONG:
      base_type = "ulong";
      break;

    case nlc::sa::BuiltinType::BUILTIN_TYPE_CHAR:
      base_type = "char";
      break;
    case nlc::sa::BuiltinType::BUILTIN_TYPE_SHORT:
      base_type = "short";
      break;
    case nlc::sa::BuiltinType::BUILTIN_TYPE_INT:
      base_type = "int";
      break;
    case nlc::sa::BuiltinType::BUILTIN_TYPE_LONG:
      base_type = "long";
      break;

    case nlc::sa::BuiltinType::BUILTIN_TYPE_FLOAT:
      base_type = "float";
      break;
    case nlc::sa::BuiltinType::BUILTIN_TYPE_DOUBLE:
      base_type = "double";
      break;

    case nlc::sa::BuiltinType::BUILTIN_TYPE_BOOL:
      base_type = "bool";
      break;

    case nlc::sa::BuiltinType::BUILTIN_TYPE_STRUCT:
    case nlc::sa::BuiltinType::BUILTIN_TYPE_UNION:
    case nlc::sa::BuiltinType::BUILTIN_TYPE_ENUM:
      base_type = type.type_name;
      break;

    case nlc::sa::BuiltinType::BUILTIN_TYPE_FUNC:
      {
        base_type = "(";
        for (size_t i = 0; i < type.argument_types.size (); i++)
          {
            if (i > 0)
              {
                base_type += ", ";
              }
            const auto &argtype = type.argument_types.at (i);
            base_type += get_type_name_from_sa_type (argtype);
          }
        base_type += ") -> ";

        const auto &return_type = *type.return_type;
        base_type += get_type_name_from_sa_type (return_type);
      }
      break;
    }

  return pointer_str + base_type;
}
