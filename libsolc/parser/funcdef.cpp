#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST
Parser::parse_function_def ()
{
  auto pos = _pos;

  auto funcproto = parse_function_prototype ();

  AST funcdef (pos, ASTType::FUNC_DEF);
  funcdef.append (funcproto);

  VERIFY_POS (_pos);
  VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::LBRACE);

  auto stmtlist = parse_statement_list ();
  funcdef.append (stmtlist);

  return funcdef;
}

}
