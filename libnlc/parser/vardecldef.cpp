#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_variable_decldef ()
{
  auto start_pos = _pos;

  auto cur = _tokens.at (_pos);
  const std::string identifier = cur.value;
  _pos++;
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_COLON);
  _pos++;
  VERIFY_POS (_pos);

  AST type_ = parse_type ();
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);

  if (cur.type == TokenType::TOKEN_EQ)
    {
      AST variable_def (start_pos, ASTType::AST_VAR_DEF, identifier);
      _pos++;
      variable_def.append (type_);

      VERIFY_POS (_pos);
      cur = _tokens.at (_pos);
      if (cur.type == TokenType::TOKEN_LBRACE)
        {
          auto initlist = parse_initialization_list ();
          variable_def.append (initlist);
        }
      else
        {
          auto expr = parse_expression ();
          variable_def.append (expr);
        }

      return variable_def;
    }

  AST variable_decl (start_pos, ASTType::AST_VAR_DECL, identifier);
  _pos++;
  variable_decl.append (type_);
  return variable_decl;
}

}
