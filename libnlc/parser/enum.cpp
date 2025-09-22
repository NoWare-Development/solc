#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_enum_definition ()
{
  AST enum_definition (_pos++, ASTType::ENUM_DEF);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_ID);

  enum_definition.value = cur.value;

  _pos++;
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_LBRACE);

  _pos++;
  VERIFY_POS (_pos);

  while (_pos < _tokens.size ())
    {
      VERIFY_POS (_pos);
      cur = _tokens.at (_pos);
      if (cur.type == TokenType::TOKEN_RBRACE)
        {
          break;
        }

      auto enum_element = parse_enum_element ();
      enum_definition.append (enum_element);
      auto next = peek (_pos);
      if (next == TokenType::TOKEN_COMMA)
        {
          _pos++;
        }
      else if (next == TokenType::TOKEN_ID)
        {
          add_error (ParserError::ErrType::PARSER_ERROR_UNEXPECTED, _pos);
          return {};
        }
    }

  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_RBRACE);
  _pos++;

  return enum_definition;
}

AST
Parser::parse_enum_element ()
{
  AST enum_element (_pos, ASTType::ENUM_ELEMENT);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_ID);
  enum_element.value = cur.value;
  _pos++;

  if (_pos < _tokens.size ())
    {
      auto next = peek (_pos);
      if (next == TokenType::TOKEN_EQ)
        {
          _pos++;
          VERIFY_POS (_pos);
          auto expr = parse_expression ();
          enum_element.append (expr);
        }
    }

  return enum_element;
}

}
