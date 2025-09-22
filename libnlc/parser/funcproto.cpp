#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_function_prototype ()
{
  AST funcproto (_pos, ASTType::FUNC_PROTO, _tokens.at (_pos).value);

  _pos += 2;
  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_LPAREN);
  auto arguments = parse_argument_list ();
  funcproto.append (arguments);

  auto next = peek (_pos);
  if (next != TokenType::TOKEN_RARROW)
    {
      return funcproto;
    }

  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_RARROW);

  _pos++;
  VERIFY_POS (_pos);
  auto return_type = parse_type ();

  funcproto.append (return_type);

  return funcproto;
}

}
