#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_function_argument ()
{
  auto cur = _tokens.at (_pos);
  if (cur.type == TokenType::TOKEN_PERIOD)
    {
      VERIFY_POS (_pos + 1);
      VERIFY_POS (_pos + 2);
      VERIFY_TOKEN (_pos + 1, peek (_pos + 1), TokenType::TOKEN_PERIOD);
      VERIFY_TOKEN (_pos + 2, peek (_pos + 2), TokenType::TOKEN_PERIOD);

      AST out (_pos, ASTType::VARIADIC);
      _pos += 3;
      return out;
    }

  if (cur.type == TokenType::TOKEN_ID && is_modifier (cur.value))
    {
      AST modifier_arg (_pos++, ASTType::MODIFIER, cur.value);
      auto argument = parse_function_argument ();
      modifier_arg.append (argument);
      return modifier_arg;
    }

  return parse_variable_decldef ();
}

}
