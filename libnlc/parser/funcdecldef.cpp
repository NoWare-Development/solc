#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_function_decldef ()
{
  auto funcproto = parse_function_prototype ();

  auto next = peek (_pos);
  if (next == TokenType::SEMI)
    {
      AST funcdecl (_pos++, ASTType::FUNC_DECL);
      funcdecl.append (funcproto);
      return funcdecl;
    }

  AST funcdef (_pos, ASTType::FUNC_DEF);
  funcdef.append (funcproto);

  VERIFY_TOKEN (_pos, next, TokenType::LBRACE);

  auto stmtlist = parse_statement_list ();
  funcdef.append (stmtlist);

  return funcdef;
}

}
