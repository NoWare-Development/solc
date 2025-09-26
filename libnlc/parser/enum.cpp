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
  VERIFY_TOKEN (_pos, cur.type, TokenType::ID);

  enum_definition.value = cur.value;

  _pos++;
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::LBRACE);

  _pos++;
  VERIFY_POS (_pos);

  while (_pos < _tokens.size ())
    {
      VERIFY_POS (_pos);
      cur = _tokens.at (_pos);
      if (cur.type == TokenType::RBRACE)
        {
          break;
        }

      auto enum_element = parse_enum_element ();
      enum_definition.append (enum_element);
      auto next = peek (_pos);
      if (next == TokenType::COMMA)
        {
          _pos++;
        }
      else if (next == TokenType::ID)
        {
          add_error (ParserError::Type::UNEXPECTED, _pos);
          return {};
        }
    }

  VERIFY_TOKEN (_pos, cur.type, TokenType::RBRACE);
  _pos++;

  return enum_definition;
}

AST
Parser::parse_enum_element ()
{
  AST enum_element (_pos, ASTType::ENUM_ELEMENT);

  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::ID);
  enum_element.value = cur.value;
  _pos++;

  if (_pos < _tokens.size ())
    {
      auto next = peek (_pos);
      if (next == TokenType::EQ)
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
