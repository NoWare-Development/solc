#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_union()
{
  VERIFY_POS(_pos);
  _pos++;

  VERIFY_POS(_pos);
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::ID);
  AST uniondef(_pos, ASTType::UNION, cur.value);
  _pos++;

  VERIFY_POS(_pos);
  cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::LBRACE);
  _pos++;

  while (_pos < _tokens.size()) {
    cur = _tokens.at(_pos);
    if (cur.type == TokenType::RBRACE) {
      break;
    }

    if (cur.type == TokenType::ID &&
        _union_parse_methods.find(cur.value) != _union_parse_methods.end()) {
      auto ast = (this->*_union_parse_methods.at(cur.value))();
      uniondef.append(ast);
      continue;
    }

    auto decldef = parse_decldef();
    uniondef.append(decldef);
  }

  VERIFY_POS(_pos);
  cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::RBRACE);
  _pos++;

  return uniondef;
}

}
