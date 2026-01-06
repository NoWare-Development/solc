#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_type()
{
  auto next = peek(_pos);

  // Parse typeof()
  if (next == TokenType::ID && _tokens.at(_pos).value == "typeof") {
    AST type_typeof(_pos, ASTType::TYPE_TYPEOF);

    _pos++;
    VERIFY_POS(_pos);
    VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::LPAREN);

    _pos++;
    VERIFY_POS(_pos);
    auto expr = parse_expression();

    VERIFY_POS(_pos);
    VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::RPAREN);
    _pos++;

    type_typeof.append(expr);
    return type_typeof;
  }

  auto type_raw = parse_type_raw();

  // Parse all nested arrays which may be after ID
  next = peek(_pos);
  AST current = type_raw;
  AST buf{};
  while (_pos < _tokens.size() && next == TokenType::LBRACK) {
    buf = AST(_pos++, ASTType::TYPE_ARRAY);

    next = peek(_pos);
    if (next != TokenType::RBRACK) {
      auto expr = parse_expression();
      buf.append(expr);
      VERIFY_POS(_pos);
      next = peek(_pos);
    }

    VERIFY_TOKEN(_pos, next, TokenType::RBRACK);
    _pos++;
    buf.append(current);
    current = buf;

    next = peek(_pos);
  }

  return current;
}

}
