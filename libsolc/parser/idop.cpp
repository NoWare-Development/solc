#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"
#include <vector>

namespace solc
{

AST Parser::parse_identifier_operand(bool accept_modules, bool accept_functions)
{
  VERIFY_POS(_pos);
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::ID);

  AST out_operand;

  auto next = peek(_pos + 1);

  if (next == TokenType::DCOLON && accept_modules) {
    AST out(_pos, ASTType::NAMESPACE, cur.value);
    _pos += 2;
    auto symbol = parse_identifier_operand(accept_modules, accept_functions);
    out.append(symbol);
    return out;
  }

  if (accept_functions) {
    if (next == TokenType::LPAREN)
      out_operand = parse_call_operand();
    else if (is_generic_call_operand())
      out_operand = parse_generic_call_operand();
  }

  if (out_operand.type == ASTType::NONE)
    out_operand = AST(_pos++, ASTType::EXPR_OPERAND_IDENTIFIER, cur.value);

  next = peek(_pos);
  if (next == TokenType::PERIOD) {
    _pos++;
    VERIFY_POS(_pos);
    auto symbol = parse_identifier_operand(false, accept_functions);

    AST access(_pos - 1, ASTType::EXPR_OPERAND_ACCESS_MEMBER);
    access.append(symbol);
    out_operand.append(access);
  }

  return out_operand;
}

}
