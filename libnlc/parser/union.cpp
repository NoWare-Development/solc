#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_union ()
{
  AST uniondef (_pos, ASTType::UNION);

  VERIFY_POS (_pos);
  _pos++;

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_ID);
  uniondef.value = cur.value;
  _pos++;

  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_LBRACE);
  _pos++;

  while (_pos < _tokens.size ())
    {
      cur = _tokens.at (_pos);
      if (cur.type == TokenType::TOKEN_RBRACE)
        {
          break;
        }

      if (cur.type == TokenType::TOKEN_ID)
        {
          if (cur.value == "struct")
            {
              auto structdef = parse_struct ();
              uniondef.append (structdef);
              continue;
            }
          else if (cur.value == "union")
            {
              auto uniondef2 = parse_union ();
              uniondef.append (uniondef2);
              continue;
            }
        }

      auto decldef = parse_decldef ();
      uniondef.append (decldef);
    }

  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_RBRACE);
  _pos++;

  return uniondef;
}

}
