#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_top()
{
  auto cur = _tokens.at(_pos);
  VERIFY_POS(_pos);
  switch (cur.type) {
  case TokenType::SEMI: {
    return AST(_pos++, ASTType::NONE);
  }

  case TokenType::ID: {
    if (_top_parse_funcs_based_on_id.find(cur.value) !=
        _top_parse_funcs_based_on_id.end()) {
      return (this->*_top_parse_funcs_based_on_id.at(cur.value))();
    } else if (auto next = peek(_pos + 1); next == TokenType::LTHAN) {
      return parse_generic_function();
    }

    return parse_decldef();
  }

  default:
    break;
  }

  add_error(ParserError::Type::UNEXPECTED, _pos++);
  return {};
}

}
