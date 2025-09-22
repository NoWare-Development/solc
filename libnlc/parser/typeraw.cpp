#include "lexer/token.hpp"
#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_type_raw ()
{
  auto cur = _tokens.at (_pos);
  if (cur.type == TokenType::TOKEN_MUL)
    {
      AST type_raw_pointer (_pos++, ASTType::TYPE_POINTER);
      VERIFY_POS (_pos);
      auto type_raw = parse_type_raw ();
      type_raw_pointer.append (type_raw);
      return type_raw_pointer;
    }
  else if (cur.type == TokenType::TOKEN_LPAREN)
    {
      auto funcptrtype = parse_function_pointer_type ();
      return funcptrtype;
    }

  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_ID);
  auto next = peek (_pos + 1);
  if (next == TokenType::TOKEN_DCOLON)
    {
      AST out (_pos, ASTType::FROM_MODULE, cur.value);
      _pos += 2;
      auto member_type = parse_type_raw ();
      out.append (member_type);
      return out;
    }

  AST out (_pos++, ASTType::TYPE_PLAIN, cur.value);
  next = peek (_pos);
  if (next == TokenType::TOKEN_LTHAN)
    {
      auto generic_type_list = parse_generic_type_list ();
      out.append (generic_type_list);
    }

  return out;
}

}
