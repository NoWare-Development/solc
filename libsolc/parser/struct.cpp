#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST
Parser::parse_struct ()
{
  _pos++;

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::ID);
  AST structure (_pos, ASTType::STRUCT, cur.value);

  _pos++;
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);

  if (cur.type == TokenType::LTHAN)
    {
      auto typelist = parse_generic_placeholder_type_list ();
      structure.append (typelist);
      VERIFY_POS (_pos);
      cur = _tokens.at (_pos);
    }

  VERIFY_TOKEN (_pos, cur.type, TokenType::LBRACE);

  _pos++;
  VERIFY_POS (_pos);

  while (_pos < _tokens.size ())
    {
      cur = _tokens.at (_pos);
      if (cur.type == TokenType::RBRACE)
        {
          break;
        }

      if (cur.type == TokenType::ID)
        {
          if (cur.value == "public" || cur.value == "private")
            {
              auto vismarker = parse_visibility_marker ();
              structure.append (vismarker);
              continue;
            }
          else if (cur.value == "typedef")
            {
              auto typedef_ = parse_typedef ();
              structure.append (typedef_);
              continue;
            }
          else if (cur.value == "enum")
            {
              auto enumdef = parse_enum_definition ();
              structure.append (enumdef);
              continue;
            }
          else if (cur.value == "struct")
            {
              auto structdef = parse_struct ();
              structure.append (structdef);
              continue;
            }
          else if (cur.value == "union")
            {
              auto uniondef = parse_union ();
              structure.append (uniondef);
              continue;
            }
        }

      auto decldef = parse_decldef ();
      structure.append (decldef);
    }
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::RBRACE);
  _pos++;

  return structure;
}

}
