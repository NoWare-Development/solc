#include "lexer/token.hpp"
#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_type_raw()
{
  auto cur = _tokens.at(_pos);
  if (cur.type == TokenType::MUL) {
    AST type_raw_pointer(_pos++, ASTType::TYPE_POINTER);
    VERIFY_POS(_pos);
    auto type_raw = parse_type_raw();
    type_raw_pointer.append(type_raw);
    return type_raw_pointer;
  } else if (cur.type == TokenType::LPAREN) {
    auto funcptrtype = parse_function_pointer_type();
    return funcptrtype;
  }

  VERIFY_TOKEN(_pos, cur.type, TokenType::ID);
  auto next = peek_token(_pos + 1);
  if (next != nullptr && next->type == TokenType::COLON &&
      !next->has_whitespace_after && peek(_pos + 2) == TokenType::COLON) {
    AST out(_pos, ASTType::NAMESPACE, cur.value);
    _pos += 3;
    auto member_type = parse_type_raw();
    out.append(member_type);
    return out;
  }

  AST out(_pos++, ASTType::TYPE_PLAIN, cur.value);
  if (peek(_pos) == TokenType::LTHAN) {
    auto generic_type_list = parse_generic_type_list();
    out.append(generic_type_list);
  }

  return out;
}

}
