#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST
Parser::parse_typedef ()
{
  _pos++;
  VERIFY_POS (_pos);
  auto type_ = parse_type ();

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::ID);
  AST typedef_ (_pos, ASTType::TYPEDEF, cur.value);
  typedef_.append (type_);

  _pos++;
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::SEMI);
  _pos++;

  return typedef_;
}

}
