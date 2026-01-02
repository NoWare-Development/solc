#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_module()
{
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::ID);

  AST module(_pos++, ASTType::MODULE, cur.value);

  auto next_tok = peek_token(_pos);
  if (next_tok != nullptr && next_tok->type == TokenType::COLON &&
      !next_tok->has_whitespace_after && peek(_pos + 1) == TokenType::COLON) {
    _pos += 2;
    VERIFY_POS(_pos);
    auto submodule = parse_module();
    module.append(submodule);
  }

  return module;
}

}
