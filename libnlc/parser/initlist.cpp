#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
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

  auto cur = _tokens.at (_pos);
  if (cur.type == TokenType::PERIOD)
    {
      VERIFY_POS (_pos);
      cur = _tokens.at (_pos);
      AST explicit_init (_pos++, ASTType::INITLIST_ENTRY_INIT_EXPLICIT);
      VERIFY_POS (_pos);

      AST target (_pos, ASTType::INITLIST_ENTRY_INIT_EXPLICIT_TARGET);
      auto target_symbol = parse_identifier_operand (false, false);
      target_symbol = parse_array_element (target_symbol);
      target.append (target_symbol);

      VERIFY_POS (_pos);
      cur = _tokens.at (_pos);
      VERIFY_TOKEN (_pos, cur.type, TokenType::EQ);
      _pos++;

      VERIFY_POS (_pos);
      cur = _tokens.at (_pos);
      if (cur.type == TokenType::LBRACE)
        {
          auto initlist = parse_initialization_list ();
          explicit_init.append (initlist);
        }
      else
        {
          auto expr = parse_expression ();
          explicit_init.append (expr);
        }

      return explicit_init;
    }

  AST init (_pos, ASTType::INITLIST_ENTRY_INIT);

  if (cur.type == TokenType::LBRACE)
    {
      auto initlist = parse_initialization_list ();
      init.append (initlist);
    }
  else
    {
      auto expr = parse_expression ();
      init.append (expr);
    }

  return init;
}

}
