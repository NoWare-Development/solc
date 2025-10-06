#include "lexer/token.hpp"
#include "libnlc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace nlc
{

AST
Parser::parse_decldef (DeclDefFlags flags)
{
  auto cur = _tokens.at (_pos);
  if (cur.type == TokenType::ID && is_modifier (cur.value))
    {
      if ((flags & DECL_DEF_MOD) == 0)
        {
          add_error (ParserError::Type::MOD_NOT_ALLOWED, _pos++);
          return {};
        }
      AST modifier_decldef (_pos++, ASTType::MODIFIER, cur.value);
      auto underlying_decldef = parse_decldef (flags);
      modifier_decldef.append (underlying_decldef);
      return modifier_decldef;
    }

  auto next = peek (_pos + 1);
  switch (next)
    {
    case TokenType::COLON:
      {
        if ((flags & (DECL_DEF_DECL | DECL_DEF_DEF)) != 0)
          {
            auto var_decldef = parse_variable_decldef (flags);
            VERIFY_POS (_pos);
            VERIFY_TOKEN (_pos, _tokens.at (_pos).type, TokenType::SEMI);
            _pos++;
            return var_decldef;
          }
        add_error (ParserError::Type::DECL_NOT_ALLOWED, _pos++);
        return {};
      }

    case TokenType::DCOLON:
      {
        if ((flags & (DECL_DEF_FUNC)) != 0)
          {
            return parse_function_def ();
          }
        add_error (ParserError::Type::FUNC_NOT_ALLOWED, _pos++);
        return {};
      }

    case TokenType::ERR:
      {
        add_error (ParserError::Type::EXPECTED, _pos++);
        return {};
      }

    default:
      break;
    }

  add_error (ParserError::Type::UNEXPECTED, _pos++);
  return {};
}

}
