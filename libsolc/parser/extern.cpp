#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

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
  cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::COLON);
  VERIFY_WHITESPACE(_pos, cur.has_whitespace_after, false, TokenType::COLON);

  _pos++;
  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, peek(_pos), TokenType::COLON);

  _pos++;
  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::LPAREN);

  extern_func.append(parse_argument_list());

  auto next = peek_token(_pos);
  if (next != nullptr && next->type == TokenType::SUB &&
      peek(_pos + 1) == TokenType::GTHAN) {
    _pos += 2;
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
