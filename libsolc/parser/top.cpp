#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_top(bool for_export)
{
  auto cur = _tokens.at(_pos);
  VERIFY_POS(_pos);
  switch (cur.type) {
  case TokenType::SEMI: {
    return AST(_pos++, ASTType::NONE);
  }

  case TokenType::ID: {
    if (cur.value == "enum") {
      return parse_enum_definition();
    } else if (cur.value == "typedef") {
      return parse_typedef();
    } else if (cur.value == "struct") {
      return parse_struct();
    } else if (cur.value == "union") {
      return parse_union();
    } else if (!for_export) {
      if (cur.value == "import") {
        return parse_import();
      } else if (cur.value == "export") {
        return parse_export();
      } else if (auto next = peek(_pos + 1); next == TokenType::LTHAN) {
        return parse_generic_function();
      }
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
