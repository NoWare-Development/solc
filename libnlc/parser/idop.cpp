#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_identifier_operand (bool accept_modules, bool accept_functions)
{
  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_ID);

  AST out_operand;

  auto next = peek (_pos + 1);
  if (next == TokenType::TOKEN_DCOLON && accept_modules)
    {
      AST out (_pos, ASTType::FROM_MODULE, cur.value);
      _pos += 2;
      auto symbol
          = parse_identifier_operand (accept_modules, accept_functions);
      out.append (symbol);
      return out;
    }
  else if (next == TokenType::TOKEN_LPAREN && accept_functions)
    {
      out_operand = parse_call_operand ();
    }
  else
    {
      out_operand = AST (_pos++, ASTType::EXPR_OPERAND_IDENTIFIER, cur.value);
    }

  next = peek (_pos);
  if (next == TokenType::TOKEN_PERIOD)
    {
      _pos++;
      VERIFY_POS (_pos);
      auto symbol
          = parse_identifier_operand (accept_modules, accept_functions);

      if (out_operand.type == ASTType::EXPR_OPERAND_CALL)
        {
          AST access (_pos - 1, ASTType::EXPR_OPERAND_ACCESS_MEMBER);
          access.append (symbol);
          out_operand.append (access);
        }
      else
        {
          out_operand.type = ASTType::EXPR_OPERAND_ACCESS_MEMBER;
          out_operand.append (symbol);
        }
    }

  return out_operand;
}

}
