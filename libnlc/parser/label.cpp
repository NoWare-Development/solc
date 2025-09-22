#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_label ()
{
  auto cur = _tokens.at (_pos);
  auto start = _pos;
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_AT);
  _pos++;
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_ID);
  _pos++;
  return AST (start, ASTType::LABEL, cur.value);
}

}
