#include "lexer/token.hpp"
#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_decldef ()
{
  auto cur = _tokens.at (_pos);
  if (cur.type == TokenType::TOKEN_ID && is_modifier (cur.value))
    {
      AST modifier_decldef (_pos++, ASTType::MODIFIER, cur.value);
      auto underlying_decldef = parse_decldef ();
      modifier_decldef.append (underlying_decldef);
      return modifier_decldef;
    }

  auto next = peek (_pos + 1);
  switch (next)
    {
    case TokenType::TOKEN_COLON:
      {
        auto var_decldef = parse_variable_decldef ();
        VERIFY_POS (_pos);
        VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::TOKEN_SEMI);
        _pos++;
        return var_decldef;
      }

    case TokenType::TOKEN_DCOLON:
      {
        return parse_function_decldef ();
      }

    case TokenType::TOKEN_ERR:
      {
        add_error (ParserError::ErrType::PARSER_ERROR_EXPECTED, _pos);
        return {};
      }

    default:
      break;
    }

  add_error (ParserError::ErrType::PARSER_ERROR_UNEXPECTED, _pos++);
  return {};
}

}
