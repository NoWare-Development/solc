#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_function_pointer_type()
{
  AST funcptrtype(_pos, ASTType::TYPE_FUNCPTR);

  VERIFY_POS(_pos);
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::LPAREN);
  auto arglist = parse_argument_list();
  funcptrtype.append(arglist);

  if (auto next = peek(_pos); next != TokenType::SUB) {
    return funcptrtype;
  }

  VERIFY_POS(_pos);
  cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::SUB);
  VERIFY_WHITESPACE(_pos, cur.has_whitespace_after, false, TokenType::GTHAN);

  _pos++;
  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, peek(_pos), TokenType::GTHAN);

  _pos++;
  VERIFY_POS(_pos);
  auto typeraw = parse_type_raw();
  funcptrtype.append(typeraw);

  return funcptrtype;
}

}
