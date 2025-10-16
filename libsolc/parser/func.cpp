#include "lexer/token.hpp"
#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST
Parser::parse_function ()
{
  VERIFY_POS (_pos);
  VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::ID);
  AST func (_pos, ASTType::FUNC, _tokens.at (_pos).value);

  _pos++;
  VERIFY_POS (_pos);
  VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::DCOLON);

  _pos++;
  VERIFY_POS (_pos);
  VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::LPAREN);

  func.append (parse_argument_list ());

  // parse_argument_list checks if there's TokenType::RPAREN and advances past
  // it.

  auto next = peek (_pos);
  if (next == TokenType::RARROW)
    {
      _pos++;
      VERIFY_POS (_pos);
      auto return_type = parse_type ();

      func.append (return_type);
    }

  VERIFY_POS (_pos);
  VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::LBRACE);

  func.append (parse_statement_list ());

  return func;
}

AST
Parser::parse_generic_function ()
{
  VERIFY_POS (_pos);
  VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::ID);
  AST func (_pos, ASTType::GENERIC_FUNC, _tokens.at (_pos).value);

  _pos++;
  VERIFY_POS (_pos);
  VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::LTHAN);

  auto typelist = parse_generic_placeholder_type_list ();
  func.append (typelist);

  VERIFY_POS (_pos);
  VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::DCOLON);

  _pos++;
  VERIFY_POS (_pos);
  VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::LPAREN);

  func.append (parse_argument_list ());

  // parse_argument_list checks if there's TokenType::RPAREN and advances past
  // it.

  auto next = peek (_pos);
  if (next == TokenType::RARROW)
    {
      _pos++;
      VERIFY_POS (_pos);
      auto return_type = parse_type ();

      func.append (return_type);
    }

  VERIFY_POS (_pos);
  VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::LBRACE);

  func.append (parse_statement_list ());

  return func;
}

}
