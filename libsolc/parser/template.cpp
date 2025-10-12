#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST
Parser::parse_template ()
{
  AST template_ (_pos++, ASTType::TEMPLATE);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::LTHAN);
  _pos++;

  AST type_list (_pos, ASTType::TEMPLATE_TYPE_LIST);
  while (_pos < _tokens.size ())
    {
      cur = _tokens.at (_pos);
      VERIFY_TOKEN (_pos, cur.type, TokenType::ID);
      type_list.append (AST (_pos, ASTType::TEMPLATE_TYPE, cur.value));

      _pos++;
      auto next = peek (_pos);
      if (next == TokenType::GTHAN)
        {
          break;
        }

      VERIFY_POS (_pos);
      VERIFY_TOKEN (_pos, next, TokenType::COMMA);
      _pos++;
    }
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::GTHAN);
  _pos++;

  template_.append (type_list);

  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::ID);
  if (cur.value == "struct")
    {
      auto structure = parse_struct ();
      template_.append (structure);
      return template_;
    }

  VERIFY_POS (_pos + 1);
  cur = _tokens.at (_pos + 1);
  VERIFY_TOKEN (_pos, cur.type, TokenType::DCOLON);
  auto func_decldef = parse_function_def ();
  template_.append (func_decldef);

  return template_;
}

}
