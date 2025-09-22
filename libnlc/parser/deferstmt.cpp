#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_defer_statement ()
{
  AST defer_statement (_pos++, ASTType::STMT_DEFER);

  VERIFY_POS (_pos);

  auto statement = parse_statement ();
  defer_statement.append (statement);

  return defer_statement;
}

}
