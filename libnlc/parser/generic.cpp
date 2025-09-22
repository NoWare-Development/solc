#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_generic_type_list ()
{
  AST gtl (_pos, ASTType::GENERIC_TYPE_LIST);

  VERIFY_POS (_pos);
  auto cur = peek (_pos);
  VERIFY_TOKEN (_pos, cur, TokenType::TOKEN_LTHAN);
  _pos++;

  while (_pos < _tokens.size ())
    {
      cur = peek (_pos);
      if (cur == TokenType::TOKEN_GTHAN)
        {
          break;
        }

      auto type = parse_type ();
      gtl.append (type);

      auto next = peek (_pos);
      if (next != TokenType::TOKEN_GTHAN)
        {
          VERIFY_POS (_pos);
          VERIFY_TOKEN (_pos, next, TokenType::TOKEN_COMMA);
          _pos++;
        }
    }

  VERIFY_POS (_pos);
  cur = peek (_pos);
  VERIFY_TOKEN (_pos, cur, TokenType::TOKEN_GTHAN);
  _pos++;

  return gtl;
}

}
