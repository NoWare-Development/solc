#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_typedef ()
{
  AST typedef_ (_pos++, ASTType::TYPEDEF);

  VERIFY_POS (_pos);
  auto type_ = parse_type ();

  typedef_.append (type_);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_ID);
  typedef_.value = cur.value;

  _pos++;
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_SEMI);
  _pos++;

  return typedef_;
}

}
