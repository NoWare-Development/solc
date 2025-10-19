#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_module()
{
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::ID);

  AST module(_pos++, ASTType::MODULE, cur.value);

  auto next = peek(_pos);
  if (next == TokenType::DCOLON) {
    _pos++;
    VERIFY_POS(_pos);
    auto submodule = parse_module();
    module.append(submodule);
  }

  return module;
}

}
