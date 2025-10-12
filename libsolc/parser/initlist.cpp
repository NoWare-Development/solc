#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST
Parser::parse_initialization_list ()
{
  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::LBRACE);
  AST initlist (_pos, ASTType::INITLIST);
  _pos++;

  while (_pos < _tokens.size ())
    {
      cur = _tokens.at (_pos);
      if (cur.type == TokenType::RBRACE)
        {
          break;
        }

      auto entry = parse_initialization_list_entry ();
      initlist.append (entry);
      auto next = peek (_pos);
      if (next != TokenType::RBRACE)
        {
          VERIFY_POS (_pos);
          VERIFY_TOKEN (_pos, next, TokenType::COMMA);
          _pos++;
        }
    }

  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::RBRACE);

  _pos++;

  return initlist;
}

AST
Parser::parse_initialization_list_entry ()
{
  VERIFY_POS (_pos);

  auto current = _tokens.at (_pos);
  AST entry;
  if (current.type == TokenType::PERIOD)
    {
      _pos++;

      VERIFY_POS (_pos);
      current = _tokens.at (_pos);
      VERIFY_TOKEN (_pos, current.type, TokenType::ID);
      entry = AST (_pos, ASTType::INITLIST_ENTRY_EXPLICIT, current.value);

      _pos++;
      VERIFY_POS (_pos);
      current = _tokens.at (_pos);
      if (current.type == TokenType::LBRACK)
        {
          entry.type = ASTType::INITLIST_ENTRY_EXPLICIT_ARRAY_ELEM;

          _pos++;
          auto expr = parse_expression ();
          entry.append (expr);

          VERIFY_POS (_pos);
          current = _tokens.at (_pos);
          VERIFY_TOKEN (_pos, current.type, TokenType::RBRACK);

          _pos++;
          VERIFY_POS (_pos);
          current = _tokens.at (_pos);
        }
      VERIFY_TOKEN (_pos, current.type, TokenType::EQ);

      _pos++;
      VERIFY_POS (_pos);
      current = _tokens.at (_pos);
    }
  else
    {
      entry = AST (_pos, ASTType::INITLIST_ENTRY);
    }

  if (current.type == TokenType::LBRACE)
    {
      entry.append (parse_initialization_list ());
    }
  else
    {
      entry.append (parse_expression ());
    }

  return entry;
}

}
