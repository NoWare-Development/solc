#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_variable_decldef (DeclDefFlags flags)
{
  auto start_pos = _pos;

  auto cur = _tokens.at (_pos);
  const std::string identifier = cur.value;
  _pos++;
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::COLON);
  _pos++;
  VERIFY_POS (_pos);

  AST type_ = parse_type ();
  VERIFY_POS (_pos);
  cur = _tokens.at (_pos);

  if (cur.type == TokenType::EQ)
    {
      if ((flags & DECL_DEF_DEF) != 0)
        {
          AST variable_def (start_pos, ASTType::VAR_DEF, identifier);
          _pos++;
          variable_def.append (type_);

          VERIFY_POS (_pos);
          cur = _tokens.at (_pos);
          if (cur.type == TokenType::LBRACE)
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

      add_error (ParserError::Type::DEF_NOT_ALLOWED, _pos++);
      return {};
    }

  if ((flags & DECL_DEF_DECL) == 0)
    {
      add_error (ParserError::Type::DECL_NOT_ALLOWED, _pos++);
      return {};
    }

  AST variable_decl (start_pos, ASTType::VAR_DECL, identifier);
  variable_decl.append (type_);
  return variable_decl;
}

}
