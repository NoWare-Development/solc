#include "lexer/token.hpp"
#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST
Parser::parse_decldef ()
{
  auto cur = _tokens.at (_pos);
  if (cur.type == TokenType::ID && is_modifier (cur.value))
    {
      AST modifier_decldef (_pos++, ASTType::MODIFIER, cur.value);
      auto underlying_decldef = parse_decldef ();
      modifier_decldef.append (underlying_decldef);
      return modifier_decldef;
    }

  auto next = peek (_pos + 1);
  switch (next)
    {
    case TokenType::COLON:
      {
        auto var_decldef = parse_variable_decldef ();
        VERIFY_POS (_pos);
        VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::SEMI);
        _pos++;
        return var_decldef;
      }

    case TokenType::DCOLON:
      {
        return parse_function_def ();
      }

    case TokenType::ERR:
      {
        add_error (ParserError::Type::EXPECTED, _pos++);
        return {};
      }

    default:
      break;
    }

  add_error (ParserError::Type::UNEXPECTED, _pos++);
  return {};
}

}
