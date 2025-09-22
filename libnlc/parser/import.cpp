#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_import ()
{
  AST import (_pos++, ASTType::IMPORT);

  VERIFY_POS (_pos);
  auto module = parse_module ();
  import.append (module);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_SEMI);
  _pos++;

  return import;
}

}
