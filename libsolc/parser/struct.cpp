#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_struct()
{
  _pos++;

  VERIFY_POS(_pos);
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::ID);
  AST structure(_pos, ASTType::STRUCT, cur.value);

  _pos++;
  VERIFY_POS(_pos);
  cur = _tokens.at(_pos);

  if (cur.type == TokenType::LTHAN) {
    auto typelist = parse_generic_placeholder_type_list();
    structure.append(typelist);
    VERIFY_POS(_pos);
    cur = _tokens.at(_pos);
  }

  VERIFY_TOKEN(_pos, cur.type, TokenType::LBRACE);

  _pos++;
  VERIFY_POS(_pos);

  while (_pos < _tokens.size()) {
    cur = _tokens.at(_pos);
    if (cur.type == TokenType::RBRACE) {
      break;
    }

    if (cur.type == TokenType::ID &&
        _struct_parse_methods.find(cur.value) != _struct_parse_methods.end()) {
      auto ast = (this->*_struct_parse_methods.at(cur.value))();
      structure.append(ast);
      continue;
    }

    auto decldef = parse_decldef();
    structure.append(decldef);
  }
  VERIFY_POS(_pos);
  cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::RBRACE);
  _pos++;

  return structure;
}

}
