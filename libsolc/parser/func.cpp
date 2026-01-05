#include "lexer/token.hpp"
#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_function()
{
  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::ID);
  AST func(_pos, ASTType::FUNC, _tokens.at(_pos).value);

  _pos++;
  VERIFY_POS(_pos);
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::COLON);
  VERIFY_WHITESPACE(_pos, cur.has_whitespace_after, false, TokenType::COLON);
  _pos++;
  VERIFY_TOKEN(_pos, peek(_pos), TokenType::COLON);

  _pos++;
  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::LPAREN);

  func.append(parse_argument_list());

  // parse_argument_list checks if there's TokenType::RPAREN and advances past
  // it.

  auto next = peek_token(_pos);
  if (next != nullptr && next->type == TokenType::SUB &&
      !next->has_whitespace_after && peek(_pos + 1) == TokenType::GTHAN) {
    _pos += 2;
    VERIFY_POS(_pos);
    auto return_type = parse_type();

    func.append(return_type);
  }

  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::LBRACE);

  func.append(parse_statement_list());

  return func;
}

AST Parser::parse_generic_function()
{
  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::ID);
  AST func(_pos, ASTType::GENERIC_FUNC, _tokens.at(_pos).value);

  _pos++;
  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::LTHAN);

  auto typelist = parse_generic_placeholder_type_list();
  func.append(typelist);

  VERIFY_POS(_pos);
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::COLON);
  VERIFY_WHITESPACE(_pos, cur.has_whitespace_after, false, TokenType::COLON);
  _pos++;
  VERIFY_TOKEN(_pos, peek(_pos), TokenType::COLON);

  _pos++;
  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::LPAREN);

  func.append(parse_argument_list());

  // parse_argument_list checks if there's TokenType::RPAREN and advances past
  // it.

  auto next = peek_token(_pos);
  if (next != nullptr && next->type == TokenType::SUB &&
      !next->has_whitespace_after && peek(_pos + 1) == TokenType::GTHAN) {
    _pos += 2;
    VERIFY_POS(_pos);
    auto return_type = parse_type();

    func.append(return_type);
  }

  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, _tokens.at(_pos).type, TokenType::LBRACE);

  func.append(parse_statement_list());

  return func;
}

}
