#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST
Parser::parse_array_element (AST array)
{
  auto curtype = peek (_pos);
  AST current = array;
  AST buf{};
  while (curtype == TokenType::LBRACK && _pos < _tokens.size ())
    {
      _pos++;

      VERIFY_POS (_pos);
      buf = AST (_pos, ASTType::EXPR_OPERAND_ARRAY_ELEMENT);

      auto expr = parse_expression ();
      buf.append (expr);
      buf.append (current);
      VERIFY_POS (_pos);
      curtype = peek (_pos);
      VERIFY_TOKEN (_pos, curtype, TokenType::RBRACK);
      _pos++;

      current = buf;

      curtype = peek (_pos);
    }

  return current;
}

}
