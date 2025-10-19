#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_visibility_marker()
{
  AST vismarker;

  VERIFY_POS(_pos);
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::ID);

  if (cur.value == "public") {
    vismarker = AST(_pos, ASTType::VISIBILITY_MARKER_PUBLIC);
  } else {
    VERIFY_VALUE(_pos, cur.value, "private");
    vismarker = AST(_pos, ASTType::VISIBILITY_MARKER_PRIVATE);
  }

  _pos++;
  VERIFY_POS(_pos);
  cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::COLON);
  _pos++;

  return vismarker;
}

}
