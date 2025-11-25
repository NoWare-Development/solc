#include "lexer/token.hpp"
#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_return_statement()
{
  AST return_statement(_pos++, ASTType::STMT_RETURN);
  VERIFY_POS(_pos);

  auto cur = _tokens.at(_pos);
  if (cur.type == TokenType::SEMI) {
    _pos++;
    return return_statement;
  } else if (cur.type == TokenType::LBRACE) {
    auto initlist = parse_initialization_list();
    return_statement.append(initlist);
  } else {
    auto expr = parse_expression();
    return_statement.append(expr);
  }
  VERIFY_POS(_pos);
  cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::SEMI);
  _pos++;

  return return_statement;
}

}
