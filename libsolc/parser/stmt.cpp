#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

AST Parser::parse_statement_list()
{
  AST stmtlist(_pos, ASTType::STMT_LIST);
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::LBRACE);
  _pos++;
  VERIFY_POS(_pos);

  while (_pos < _tokens.size()) {
    cur = _tokens.at(_pos);
    if (cur.type == TokenType::RBRACE) {
      break;
    }

    auto statement = parse_statement();
    stmtlist.append(statement);
  }

  VERIFY_POS(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::RBRACE);
  _pos++;
  return stmtlist;
}

AST Parser::parse_statement()
{
  auto cur = _tokens.at(_pos);
  switch (cur.type) {
  case TokenType::SEMI: {
    return AST(_pos++, ASTType::NONE);
  }

  case TokenType::LBRACE: {
    return parse_statement_list();
  }

  case TokenType::ID: {
    // What is this syntax, bro, I'm crying...
    if (_stmt_parse_methods.find(cur.value) != _stmt_parse_methods.end()) {
      return (this->*_stmt_parse_methods.at(cur.value))();
    } else if (is_modifier(cur.value)) {
      return parse_decldef();
    }

    auto next = peek(_pos + 1);
    switch (next) {
    case TokenType::DCOLON:
      // Check if statement is function declaration/definition.
      // If not, it is access from other module and should be treated as
      // expression statement.
      next = peek(_pos + 2);
      if (next != TokenType::LPAREN) {
        break;
      }
    case TokenType::COLON:
      return parse_decldef();
    default:
      break;
    }

    break;
  }

  case TokenType::AT: {
    return parse_label_statement();
  }

  default:
    break;
  }

  // Can be expression statement, a generic function definition
  // or a generic namespace.
  auto out = parse_expression_statement_or_generic_func_or_namespace();
  if (_errored) {
    _pos++;
    return {};
  }

  return out;
}

AST Parser::parse_expression_statement_or_generic_func_or_namespace()
{
  auto pos = _pos;

  VERIFY_POS(pos);
  auto cur = _tokens.at(pos);
  if (cur.type == TokenType::ID) {
    pos++;
    if (peek(pos++) == TokenType::LTHAN) {
      bool not_a_generic_func = false;
      while (pos < _tokens.size() && !not_a_generic_func) {
        const auto &tok = _tokens.at(pos);
        switch (tok.type) {
        case TokenType::ID:
        case TokenType::COMMA:
          break;

        case TokenType::GTHAN: {
          if (peek(pos + 1) == TokenType::DCOLON) {
            auto next = peek(pos + 2);
            if (next == TokenType::LPAREN) {
              return parse_generic_function();
            } else if (next == TokenType::ID) {
              auto generic_namespace = parse_generic_namespace();
              auto symbol = parse_identifier_operand();
              generic_namespace.append(symbol);
              return generic_namespace;
            } else if (next == TokenType::ERR) {
              add_error(ParserError::Type::EXPECTED, _pos + 2);
              return {};
            } else {
              add_error(ParserError::Type::UNEXPECTED, _pos + 2);
              return {};
            }
          } else {
            not_a_generic_func = true;
          }
        } break;

        default:
          not_a_generic_func = true;
          break;
        }

        pos++;
      }
    }
  }

  return parse_expression_statement();
}

}
