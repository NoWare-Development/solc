#include "lexer/token.hpp"
#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_decldef()
{
  auto cur = _tokens.at(_pos);
  if (is_qualifier(cur.value)) {
    AST modifier_decldef(_pos++, ASTType::QUALIFIER, cur.value);
    auto underlying_decldef = parse_decldef();
    modifier_decldef.append(underlying_decldef);
    return modifier_decldef;
  }

  if (cur.type == TokenType::ID && cur.value == "func") {
    AST explicit_func;

    _pos++;
    VERIFY_POS(_pos);
    VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::ID);

    if (peek(_pos + 1) == TokenType::LTHAN) {
      explicit_func = parse_generic_function();
      explicit_func.type = ASTType::GENERIC_FUNC_EXPLICIT;
    } else {
      explicit_func = parse_function();
      explicit_func.type = ASTType::FUNC_EXPLICIT;
    }

    return explicit_func;
  }

  auto next = peek_token(_pos + 1);
  if (next == nullptr) {
    add_error(ParserError::Type::EXPECTED, _pos++);
    return {};
  }

  switch (next->type) {
  case TokenType::COLON: {
    if (!next->has_whitespace_after && peek(_pos + 2) == TokenType::COLON) {
      return parse_function();
    }

    auto var_decldef = parse_variable_decldef();
    VERIFY_POS(_pos);
    VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::SEMI);
    _pos++;
    return var_decldef;
  }

  case TokenType::LTHAN: {
    return parse_generic_function();
  } break;

  default:
    break;
  }

  add_error(ParserError::Type::UNEXPECTED, _pos++);
  return {};
}
}
