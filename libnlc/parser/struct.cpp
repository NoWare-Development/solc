#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_struct ()
{
  AST structure (_pos++, ASTType::STRUCT);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_ID);
  structure.value = cur.value;

  _pos++;
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_LBRACE);

  _pos++;
  VERIFY_POS (_pos);

  while (_pos < _tokens.size ())
    {
      cur = _tokens.at (_pos);
      if (cur.type == TokenType::TOKEN_RBRACE)
        {
          break;
        }

      if (cur.type == TokenType::TOKEN_ID)
        {
          if (cur.value == "template")
            {
              auto template_ = parse_template ();
              structure.append (template_);
              continue;
            }
          else if (cur.value == "public" || cur.value == "private")
            {
              auto vismarker = parse_visibility_marker ();
              structure.append (vismarker);
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
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_RBRACE);
  _pos++;

  return structure;
}

}
