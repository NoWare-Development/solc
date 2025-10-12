#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST
Parser::parse_label_statement ()
{
  AST label_statement (_pos, ASTType::STMT_LABEL);
  VERIFY_POS (_pos);
  auto label = parse_label ();
  label_statement.append (label);
  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::COLON);
  _pos++;
  return label_statement;
}

}
