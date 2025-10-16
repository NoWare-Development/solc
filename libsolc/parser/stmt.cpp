#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST
Parser::parse_statement_list ()
{
  AST stmtlist (_pos, ASTType::STMT_LIST);
  auto cur = _tokens.at (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::LBRACE);
  _pos++;
  VERIFY_POS (_pos);

  while (_pos < _tokens.size ())
    {
      cur = _tokens.at (_pos);
      if (cur.type == TokenType::RBRACE)
        {
          break;
        }

      auto statement = parse_statement ();
      stmtlist.append (statement);
    }

  VERIFY_POS (_pos);
  VERIFY_TOKEN (_pos, cur.type, TokenType::RBRACE);
  _pos++;
  return stmtlist;
}

AST
Parser::parse_statement ()
{
  auto cur = _tokens.at (_pos);
  switch (cur.type)
    {
    case TokenType::SEMI:
      {
        return AST (_pos++, ASTType::NONE);
      }

    case TokenType::LBRACE:
      {
        return parse_statement_list ();
      }

    case TokenType::ID:
      {
        if (cur.value == "struct")
          {
            return parse_struct ();
          }
        else if (cur.value == "union")
          {
            return parse_union ();
          }
        else if (cur.value == "enum")
          {
            return parse_enum_definition ();
          }
        else if (cur.value == "return")
          {
            return parse_return_statement ();
          }
        else if (cur.value == "goto")
          {
            return parse_goto_statement ();
          }
        else if (cur.value == "break")
          {
            return parse_break_statement ();
          }
        else if (cur.value == "continue")
          {
            return parse_continue_statement ();
          }
        else if (cur.value == "while")
          {
            return parse_while_statement ();
          }
        else if (cur.value == "for")
          {
            return parse_for_statement ();
          }
        else if (cur.value == "do")
          {
            return parse_do_while_statement ();
          }
        else if (cur.value == "switch")
          {
            return parse_switch_statement ();
          }
        else if (cur.value == "defer")
          {
            return parse_defer_statement ();
          }
        else if (cur.value == "if")
          {
            return parse_if_statement ();
          }
        else if (is_modifier (cur.value))
          {
            return parse_decldef ();
          }

        auto next = peek (_pos + 1);
        switch (next)
          {
          case TokenType::DCOLON:
            // Check if statement is function declaration/definition.
            // If not, it is access from other module and should be treated as
            // expression statement.
            next = peek (_pos + 2);
            if (next != TokenType::LPAREN)
              {
                break;
              }
          case TokenType::COLON:
            return parse_decldef ();
          default:
            break;
          }

        break;
      }

    case TokenType::AT:
      {
        return parse_label_statement ();
      }

    default:
      break;
    }

  // Can be expression statement or a generic function definition.
  auto out = parse_expression_statement_or_generic_func ();
  if (_errored)
    {
      _pos++;
      return {};
    }

  return out;
}

AST
Parser::parse_expression_statement_or_generic_func ()
{
  auto pos = _pos;

  VERIFY_POS (pos);
  auto cur = _tokens.at (pos);
  if (cur.type == TokenType::ID)
    {
      pos++;
      if (peek (pos++) == TokenType::LTHAN)
        {
          bool not_a_generic_func = false;
          while (pos < _tokens.size () && !not_a_generic_func)
            {
              const auto &tok = _tokens.at (pos);
              switch (tok.type)
                {
                case TokenType::ID:
                case TokenType::COMMA:
                  break;

                case TokenType::GTHAN:
                  {
                    if (peek (pos + 1) == TokenType::DCOLON)
                      {
                        return parse_generic_function ();
                      }
                    else
                      {
                        not_a_generic_func = true;
                      }
                  }
                  break;

                default:
                  not_a_generic_func = true;
                  break;
                }

              pos++;
            }
        }
    }

  return parse_expression_statement ();
}

}
