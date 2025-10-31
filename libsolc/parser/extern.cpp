#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"
#include "sa/sa.hpp"

namespace solc
{

AST Parser::parse_extern()
{
  _pos++;
  VERIFY_POS(_pos);
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::ID);
  AST extern_func(_pos, ASTType::EXTERN_FUNC, cur.value);

  _pos++;
  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::DCOLON);

  _pos++;
  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::LPAREN);

  extern_func.append(parse_argument_list());

  auto next = peek(_pos);
  if (next == TokenType::RARROW) {
    _pos++;
    VERIFY_POS(_pos);
    auto return_type = parse_type();

    extern_func.append(return_type);
  }

  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::SEMI);
  _pos++;

  return extern_func;
}

}
