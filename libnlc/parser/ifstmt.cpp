#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_if_statement ()
{
  AST ifstmt (_pos++, ASTType::STMT_IF);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_LPAREN);
  _pos++;

  VERIFY_POS (_pos);
  auto expr = parse_expression ();
  ifstmt.append (expr);

  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_RPAREN);
  _pos++;

  auto stmt = parse_statement ();
  ifstmt.append (stmt);

  auto next = peek (_pos);
  if (next == TokenType::TOKEN_ID)
    {
      cur = _tokens.at (_pos);
      if (cur.value == "else")
        {
          auto elsestmt = parse_else_statement ();
          ifstmt.append (elsestmt);
        }
    }

  return ifstmt;
}

AST
Parser::parse_else_statement ()
{
  AST elsestmt (_pos, ASTType::STMT_ELSE);

  _pos++;
  VERIFY_POS (_pos);

  auto stmt = parse_statement ();

  elsestmt.append (stmt);

  return elsestmt;
}

}
