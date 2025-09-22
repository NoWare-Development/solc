#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_function_pointer_type ()
{
  AST funcptrtype (_pos, ASTType::TYPE_FUNCPTR);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_LPAREN);
  auto arglist = parse_argument_list ();
  funcptrtype.append (arglist);

  auto next = peek (_pos);
  if (next != TokenType::TOKEN_RARROW)
    {
      return funcptrtype;
    }

  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_RARROW);

  _pos++;
  VERIFY_POS (_pos);
  auto typeraw = parse_type_raw ();
  funcptrtype.append (typeraw);

  return funcptrtype;
}

}
