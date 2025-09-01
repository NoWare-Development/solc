#include "libnlc/parser/macros.hpp"
#include "parser/ast.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace nlc
{

AST
Parser::parse_expression_statement ()
{
  AST expr_stmt (_pos, ASTType::AST_STMT_EXPR);
  auto expr = parse_expression (true);
  expr_stmt.append (expr);
  VERIFY_POS (_pos);
  auto cur = peek (_pos);
  VERIFY_TOKEN (_pos, cur, TokenType::TOKEN_SEMI);
  _pos++;
  return expr_stmt;
}

AST
Parser::parse_expression (bool toplevel)
{
  VERIFY_POS (_pos);
  size_t start = _pos;
  auto expr_tree = parse_expression_tree (toplevel);
  if (expr_tree.is_empty ())
    {
      add_error (ParserError::ErrType::PARSER_ERROR_UNEXPECTED, _pos);
      return {};
    }

  size_t invalid_pos;
  std::string error_reason{};
  if (!validate_expression (expr_tree, invalid_pos, error_reason))
    {
      AST out (start, ASTType::AST_EXPR);
      out.append (AST (invalid_pos, ASTType::AST_ERR, error_reason));
      return out;
    }

  return pratt_parse_expression (expr_tree.children, nullptr, 0);
}

AST
Parser::parse_expression_tree (bool toplevel)
{
  AST expr_tree (_pos, ASTType::AST_EXPR);

  TokenType prev{ TokenType::TOKEN_ERR };
  while (_pos < _tokens.size ())
    {
      auto cur = peek (_pos);

      // Prefix operators
      if ((prev == TokenType::TOKEN_ERR || is_operator (prev))
          && is_prefix_operator (cur))
        {
          expr_tree.append (AST (_pos++, _prefix_operators.at (cur)));
          prev = cur;
          continue;
        }

      // Operators
      else if (!is_operator (prev) && prev != TokenType::TOKEN_ERR)
        {
          // Binary operators
          if (is_binary_operator (cur))
            {
              expr_tree.append (AST (_pos++, _binary_operators.at (cur)));
              prev = cur;
              continue;
            }

          // Compare operators
          else if (is_compare_operator (cur))
            {
              expr_tree.append (AST (_pos++, _compare_operators.at (cur)));
              prev = cur;
              continue;
            }

          else if (is_boolean_operator (cur))
            {
              expr_tree.append (AST (_pos++, _boolean_operators.at (cur)));
              prev = cur;
              continue;
            }

          // Assignment operators
          else if (is_assign_operator (cur) && toplevel)
            {
              expr_tree.append (AST (_pos++, _assign_operators.at (cur)));
              prev = cur;
              continue;
            }
        }

      else if (is_operator (prev) || prev == TokenType::TOKEN_ERR)
        {
          if (cur == TokenType::TOKEN_ID        // Identifier
              || cur == TokenType::TOKEN_LPAREN // Nested expression
              || cur == TokenType::TOKEN_STRING
              || cur == TokenType::TOKEN_SYMBOL || is_numeric_token (cur))
            {
              auto operand = parse_expression_operand ();
              expr_tree.append (operand);
              prev = peek (_pos - 1);
              continue;
            }
        }

      break;
    }

  return expr_tree;
}

AST
Parser::parse_expression_operand ()
{
  VERIFY_POS (_pos);
  auto cur = _tokens.at (_pos);

  AST out_operand{};

  switch (cur.type)
    {
    case TokenType::TOKEN_LPAREN:
      {
        _pos++;
        VERIFY_POS (_pos);

        AST nested_expr = parse_expression ();

        VERIFY_POS (_pos);
        cur = _tokens.at (_pos);
        VERIFY_TOKEN (_pos, cur.type, TokenType::TOKEN_RPAREN);

        _pos++;

        return nested_expr;
      }

    case TokenType::TOKEN_ID:
      {
        if (cur.value == "cast")
          {
            AST cast (_pos++, ASTType::AST_EXPR_OPERAND_CAST_TO);

            VERIFY_POS (_pos);
            auto cast_token = peek (_pos);
            VERIFY_TOKEN (_pos, cast_token, TokenType::TOKEN_LPAREN);

            _pos++;
            VERIFY_POS (_pos);
            auto type_ = parse_type ();
            cast.append (type_);

            VERIFY_POS (_pos);
            cast_token = peek (_pos);
            VERIFY_TOKEN (_pos, cast_token, TokenType::TOKEN_RPAREN);

            _pos++;
            VERIFY_POS (_pos);
            auto cast_expr = parse_expression_operand ();

            cast.append (cast_expr);
            return cast;
          }

        out_operand = parse_identifier_operand ();
        break;
      }

    case TokenType::TOKEN_NUM:
    case TokenType::TOKEN_NUMHEX:
    case TokenType::TOKEN_NUMBIN:
    case TokenType::TOKEN_NUMOCT:
      {
        out_operand = number_to_operand (cur);
        _pos++;
        if (_pos < _tokens.size ())
          {
            cur = _tokens.at (_pos);
            if (cur.type == TokenType::TOKEN_ID)
              {
                out_operand.append (AST (
                    _pos++, ASTType::AST_EXPR_OPERAND_NUMTYPESPEC, cur.value));
              }
          }
        return out_operand;
      }

    case TokenType::TOKEN_NUMFLOAT:
      {
        auto value = cur.value;
        if (value.at (0) == '.')
          {
            value = '0' + value;
          }

        out_operand = AST (_pos++, ASTType::AST_EXPR_OPERAND_NUMFLOAT, value);
        if (_pos < _tokens.size ())
          {
            cur = _tokens.at (_pos);
            if (cur.type == TokenType::TOKEN_ID)
              {
                out_operand.append (AST (
                    _pos++, ASTType::AST_EXPR_OPERAND_NUMTYPESPEC, cur.value));
              }
          }
        return out_operand;
      }

    case TokenType::TOKEN_STRING:
      {
        out_operand
            = AST (_pos++, ASTType::AST_EXPR_OPERAND_STRING, cur.value);
        return out_operand;
      }

    case TokenType::TOKEN_SYMBOL:
      {
        out_operand
            = AST (_pos++, ASTType::AST_EXPR_OPERAND_SYMBOL, cur.value);
        return out_operand;
      }

    default:
      add_error (ParserError::ErrType::PARSER_ERROR_UNEXPECTED, _pos++);
      return {};
    }

  out_operand = parse_array_element (out_operand);

  auto next = peek (_pos);
  if (next == TokenType::TOKEN_PERIOD)
    {
      AST access (_pos++, ASTType::AST_EXPR_OPERAND_ACCESS_MEMBER);
      VERIFY_POS (_pos);
      auto symbol = parse_expression_operand ();
      access.append (symbol);
      out_operand.append (access);
    }

  return out_operand;
}

bool
Parser::validate_expression (const AST &expr_ast, size_t &invalid_pos,
                             std::string &out_reason) const
{
  if (expr_ast.is_empty ())
    {
      invalid_pos = expr_ast.token_position;
      out_reason = "Expression is empty";
      return false;
    }

  auto peek_ast_type
      = [] (const AST &root, int pos, size_t &tok_pos) -> ASTType {
    if (pos < 0 || pos >= root.children.size ())
      return ASTType::AST_ERR;

    tok_pos = root.children[pos].token_position;
    return root.children[pos].type;
  };

  const int children_len = expr_ast.children.size ();
  ASTType prev = ASTType::AST_ERR;
  ASTType cur = ASTType::AST_ERR;
  for (int i = 0; i < children_len; i++, prev = cur)
    {
      size_t cur_pos = 0;
      cur = peek_ast_type (expr_ast, i, cur_pos);

      if (is_prefix_operator (cur)
          && (prev == ASTType::AST_ERR || is_operator (prev)))
        {
          size_t next_pos = 0;
          auto next = peek_ast_type (expr_ast, i + 1, next_pos);
          if (is_operand (next) || is_prefix_operator (next))
            {
              continue;
            }
          invalid_pos = cur_pos;
          out_reason = "Prefix operator goes after operand";
          return false;
        }
      else if (is_operator (cur) && !is_operand (prev))
        {
          invalid_pos = cur_pos;
          out_reason = "Two operators";
          return false;
        }
      else if (is_operand (cur)
               && !(prev == ASTType::AST_ERR || is_operator (prev)))
        {
          invalid_pos = cur_pos;
          out_reason = "Two operands";
          return false;
        }
    }

  bool has_assign_operator = false;
  for (const auto &child : expr_ast.children)
    {
      auto child_type = child.type;
      if (is_assign_operator (child_type))
        {
          if (has_assign_operator)
            {
              invalid_pos = child.token_position;
              out_reason = "Two assign operators";
              return false;
            }
          has_assign_operator = true;
        }
    }

  if (!is_operand (expr_ast.children.at (children_len - 1).type))
    {
      invalid_pos = expr_ast.children.at (children_len - 1).token_position;
      out_reason = "Last node in expression is not an operand";
      return false;
    }

  return true;
}

AST
Parser::pratt_parse_expression (const std::vector<AST> &in, size_t *pos,
                                int min_bp) const
{
  bool pos_was_null = false;
  if (pos == nullptr)
    {
      pos = new size_t (0);
      pos_was_null = true;
    }
  else if (*pos >= in.size ())
    return {};

  auto lhs = in.at ((*pos)++);

  // Handle prefix operators
  if (is_prefix_operator (lhs.type))
    {
      auto start_ast_pos = lhs.token_position;
      auto cur = lhs;
      std::vector<AST> prefixlist{};

      // Obtain all prefix operators
      while (is_prefix_operator (cur.type) && (*pos) < in.size ())
        {
          prefixlist.push_back (cur);
          cur = in.at ((*pos)++);
        }
      prefixlist.push_back (cur); // obtain operand

      // Reverse prefix list
      for (size_t i = 0, j = prefixlist.size () - 1; i < j; i++, j--)
        {
          auto t = prefixlist.at (i);
          prefixlist[i] = prefixlist[j];
          prefixlist[j] = t;
        }

      // Put everything to `lhs`
      lhs = AST (cur.token_position, ASTType::AST_PREFIX_EXPR);
      for (const auto &e : prefixlist)
        {
          lhs.append (e);
        }
    }

  while (*pos < in.size ())
    {
      const auto &op = in.at (*pos);
      if (!is_operator (op.type))
        {
          std::cout << "UNEXPECTED AST AT " << std::to_string (*pos) << ":\n";
          std::cout << op.to_string () << " is not an operator\n\n";
          return {};
        }

      int l_bp, r_bp;
      get_binding_power (op.type, l_bp, r_bp);
      if (l_bp < min_bp)
        {
          break;
        }
      (*pos)++;
      auto rhs = pratt_parse_expression (in, pos, r_bp);
      AST newlhs (lhs.token_position, ASTType::AST_EXPR);
      newlhs.append (lhs);
      newlhs.append (op);
      newlhs.append (rhs);
      lhs = newlhs;
    }

  if (pos_was_null)
    {
      delete pos;
    }

  return lhs;
}

void
Parser::get_binding_power (ASTType op_type, int &l_bp, int &r_bp) const
{
  int group = (op_type & 0xFF00) >> 8;
  switch (group)
    {
    case AST_GROUP_EXPR_ASSIGN_OPERATOR:
      l_bp = 10;
      r_bp = 15;
      return;

    case AST_GROUP_EXPR_BOOLEAN_OPERATOR:
      l_bp = 60;
      r_bp = 65;
      return;

    case AST_GROUP_EXPR_COMPARE_OPERATOR:
      l_bp = 70;
      r_bp = 75;
      return;

    default:
      break;
    }

  switch (op_type)
    {
    case ASTType::AST_EXPR_BINARY_OPERATOR_ADD:
    case ASTType::AST_EXPR_BINARY_OPERATOR_SUB:
      l_bp = 20;
      r_bp = 25;
      return;

    case ASTType::AST_EXPR_BINARY_OPERATOR_MUL:
    case ASTType::AST_EXPR_BINARY_OPERATOR_DIV:
    case ASTType::AST_EXPR_BINARY_OPERATOR_MOD:
      l_bp = 30;
      r_bp = 35;
      return;

    case ASTType::AST_EXPR_BINARY_OPERATOR_SHL:
    case ASTType::AST_EXPR_BINARY_OPERATOR_SHR:
      l_bp = 40;
      r_bp = 45;
      return;

    case ASTType::AST_EXPR_BINARY_OPERATOR_AND:
    case ASTType::AST_EXPR_BINARY_OPERATOR_OR:
    case ASTType::AST_EXPR_BINARY_OPERATOR_XOR:
      l_bp = 50;
      r_bp = 55;
      return;

    default:
      break;
    }

  l_bp = 0;
  r_bp = 0;
}

}
