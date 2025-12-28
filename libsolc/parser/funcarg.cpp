#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_function_argument()
{
  auto cur = _tokens.at(_pos);
  if (cur.type == TokenType::PERIOD) {
    VERIFY_POS(_pos + 1);
    VERIFY_POS(_pos + 2);
    VERIFY_TOKEN(_pos + 1, peek(_pos + 1), TokenType::PERIOD);
    VERIFY_TOKEN(_pos + 2, peek(_pos + 2), TokenType::PERIOD);

    AST out(_pos, ASTType::VARIADIC);
    _pos += 3;
    return out;
  }

  if (cur.type == TokenType::ID && is_qualifier(cur.value)) {
    AST modifier_arg(_pos++, ASTType::QUALIFIER, cur.value);
    auto argument = parse_function_argument();
    modifier_arg.append(argument);
    return modifier_arg;
  }

  return parse_variable_decldef();
}

}
