#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST
Parser::parse_break_statement ()
{
  auto next = peek (_pos + 1);
  VERIFY_TOKEN (_pos + 1, next, TokenType::SEMI);
  AST out (_pos, ASTType::STMT_BREAK);
  _pos += 2;
  return out;
}

AST
Parser::parse_continue_statement ()
{
  auto next = peek (_pos + 1);
  VERIFY_TOKEN (_pos + 1, next, TokenType::SEMI);
  AST out (_pos, ASTType::STMT_CONTINUE);
  _pos += 2;
  return out;
}

AST
Parser::parse_while_statement ()
{
  AST while_statement (_pos++, ASTType::STMT_WHILE);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::LPAREN);
  _pos++;
  VERIFY_POS (_pos);

  auto expr = parse_expression ();
  while_statement.append (expr);
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::RPAREN);

  _pos++;
  VERIFY_POS (_pos);
  auto statement = parse_statement ();
  while_statement.append (statement);
  return while_statement;
}

AST
Parser::parse_for_statement ()
{
  AST for_statement (_pos++, ASTType::STMT_FOR);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::LPAREN);
  _pos++;
  auto statement = parse_statement ();
  for_statement.append (statement);
  VERIFY_POS (_pos);

  cur = _tokens.at (_pos);
  if (cur.type == TokenType::SEMI)
    {
      for_statement.append (AST (0, ASTType::EXPR_OPERAND_NUM, "1"));
    }
  else
    {
      auto cond_expr = parse_expression ();
      for_statement.append (cond_expr);
    }
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::SEMI);
  _pos++;
  VERIFY_POS (_pos);

  cur = _tokens.at (_pos);
  if (cur.type != TokenType::RPAREN)
    {
      auto inc_expr = parse_expression (true);
      for_statement.append (inc_expr);
    }
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::RPAREN);
  _pos++;
  VERIFY_POS (_pos);
  auto body = parse_statement ();
  for_statement.append (body);
  return for_statement;
}

AST
Parser::parse_do_while_statement ()
{
  AST do_while_statement (_pos++, ASTType::STMT_DOWHILE);

  VERIFY_POS (_pos);

  auto statement = parse_statement ();
  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::ID);
  VERIFY_VALUE (_pos, cur.value, "while");
  _pos++;
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::LPAREN);
  _pos++;
  VERIFY_POS (_pos);
  auto expr = parse_expression ();
  do_while_statement.append (expr);
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::RPAREN);
  _pos++;
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::SEMI);
  _pos++;

  do_while_statement.append (statement);

  return do_while_statement;
}

}
