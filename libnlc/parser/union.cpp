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
  VERIFY_TOKEN (_pos, cur.type, TokenType::ID);
  uniondef.value = cur.value;
  _pos++;

  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::LBRACE);
  _pos++;

  while (_pos < _tokens.size ())
    {
      cur = _tokens.at (_pos);
      if (cur.type == TokenType::RBRACE)
        {
          break;
        }

      if (cur.type == TokenType::ID)
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
          else if (cur.value == "typedef")
            {
              auto typedef_ = parse_typedef ();
              uniondef.append (typedef_);
              continue;
            }
        }

      auto decldef = parse_decldef ();
      uniondef.append (decldef);
    }

  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::RBRACE);
  _pos++;

  return uniondef;
}

}
