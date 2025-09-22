#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_switch_statement ()
{
  AST switch_statement (_pos++, ASTType::STMT_SWITCH);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_LPAREN);
  _pos++;

  VERIFY_POS (_pos);
  auto expr = parse_expression ();
  switch_statement.append (expr);

  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_RPAREN);
  _pos++;

  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_LBRACE);
  _pos++;

  while (_pos < _tokens.size ())
    {
      VERIFY_POS (_pos);
      cur = _tokens.at (_pos);
      if (cur.type == TokenType::TOKEN_RBRACE)
        {
          break;
        }

      auto case_statement = parse_case_statement ();
      if (case_statement.type == ASTType::ERR)
        {
          return {};
        }
      switch_statement.append (case_statement);
    }

  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_RBRACE);
  _pos++;

  return switch_statement;
}

}
