#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_call_operand()
{
  VERIFY_POS(_pos);
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::ID);
  AST call(_pos++, ASTType::EXPR_OPERAND_CALL, cur.value);

  VERIFY_POS(_pos);
  cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::LPAREN);

  _pos++;
  VERIFY_POS(_pos);
  while (_pos < _tokens.size()) {
    cur = _tokens.at(_pos);
    if (cur.type == TokenType::RPAREN) {
      break;
    }

    if (cur.type == TokenType::LBRACE) // Initialization list
    {
      auto initlist = parse_initialization_list();
      call.append(initlist);
    } else // Expression
    {
      auto expr = parse_expression();
      call.append(expr);
    }

    auto next = peek(_pos);
    if (next != TokenType::RPAREN) {
      VERIFY_POS(_pos);
      VERIFY_TOKEN(_pos, next, TokenType::COMMA);
      _pos++;
    }
  }

  VERIFY_POS(_pos);
  cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::RPAREN);
  _pos++;

  return call;
}

}
