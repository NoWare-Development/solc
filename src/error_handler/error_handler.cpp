#include "error_handler.hpp"
#include "../util/util.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include <iostream>
#include <string>

ErrorHandler::ErrorHandler (const std::string &filename,
                            const std::string &src)
{
  _filename = filename;
  _src = src;

  for (auto &c : _src)
    {
      if (c == '\n')
        {
          _nol++;
        }
    }

  _numoffset = get_number_length (_nol);
}

void
ErrorHandler::add_tokens (const std::vector<nlc::Token> &tokens)
{
  _tokens = tokens;
}

void
ErrorHandler::add_parser_errors (
    const std::vector<nlc::Parser::ParserError> &errors)
{
  _parser_errors = errors;
}

bool
ErrorHandler::handle_tokens () const
{
  if (has_invalid_tokens ())
    {
      print_errored_tokens ();
      return false;
    }
  return true;
}

bool
ErrorHandler::handle_parser_errors () const
{
  if (has_parser_errors ())
    {
      print_parser_errors ();
      return false;
    }

  return true;
}

bool
ErrorHandler::handle_invalid_expressions (const nlc::AST &root) const
{
  std::vector<nlc::AST> errored_asts{};
  collect_invalid_expressions (root, errored_asts);

  if (!errored_asts.empty ())
    {
      for (const auto &e : errored_asts)
        {
          if (e.token_position >= _tokens.size ())
            continue;

          const auto &token = _tokens.at (e.token_position);

          std::string message = get_message_start (
              token.line + 1, token.end - token.len + 1, "error",
              ESCColor::ESCCOLOR_RED, ESCGraphics::ESCGRAPHICS_BOLD);

          message += "Expression error: ";
          message += e.value;
          message += '\n';

          message += get_highlighted_token (token, ESCColor::ESCCOLOR_RED,
                                            ESCGraphics::ESCGRAPHICS_BOLD);
          message += '\n';

          std::cout << message;
        }
      return false;
    }

  return true;
}

void
ErrorHandler::handle_sa_errors (
    const std::vector<nlc::SemanticAnalyzer::SAError> &errors) const
{
  for (const auto &e : errors)
    {
      if (e.tok_pos >= _tokens.size ())
        continue;

      const auto &token = _tokens.at (e.tok_pos);

      std::string message = get_message_start (
          token.line + 1, token.end - token.end + 1, "error",
          ESCColor::ESCCOLOR_RED, ESCGraphics::ESCGRAPHICS_BOLD);

      message += get_sa_error_reason (e);

      message += '\n';
      message += get_highlighted_token (token, ESCColor::ESCCOLOR_RED,
                                        ESCGraphics::ESCGRAPHICS_BOLD);
      message += '\n';

      std::cout << message;
    }
}

void
ErrorHandler::collect_invalid_expressions (const nlc::AST &node,
                                           std::vector<nlc::AST> &v) const
{
  for (const auto &c : node.children)
    {
      if (c.type == nlc::ASTType::EXPR)
        {
          for (const auto &ec : c.children)
            {
              if (ec.type == nlc::ASTType::ERR)
                {
                  v.push_back (ec);
                }
            }
        }

      collect_invalid_expressions (c, v);
    }
}

void
ErrorHandler::print_errored_tokens () const
{
  auto get_invalid_tokens
      = [] (const std::vector<nlc::Token> &tokens) -> std::vector<nlc::Token> {
    std::vector<nlc::Token> invalid_tokens{};

    for (auto &tok : tokens)
      {
        if (tok.type == nlc::TokenType::ERR)
          {
            invalid_tokens.push_back (tok);
          }
      }
    return invalid_tokens;
  };

  std::vector<nlc::Token> invalid_tokens = get_invalid_tokens (_tokens);

  for (auto &tok : invalid_tokens)
    {
      std::cout << get_token_error (tok) << '\n';
    }
}

void
ErrorHandler::print_parser_errors () const
{
  for (auto &err : _parser_errors)
    {
      std::cout << get_parser_error (err) << '\n';
    }
}

std::string
ErrorHandler::get_token_error (const nlc::Token &tok) const
{
  std::string error_string{};

  error_string += get_message_start (tok.line + 1, tok.end - tok.len + 1,
                                     "error", ESCColor::ESCCOLOR_RED,
                                     ESCGraphics::ESCGRAPHICS_BOLD);
  error_string += "Unidentified token '";
  error_string += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
  error_string += tok.value;
  error_string += escape_reset ();
  error_string += "'\n";

  error_string += get_highlighted_token (tok, ESCColor::ESCCOLOR_RED,
                                         ESCGraphics::ESCGRAPHICS_BOLD);

  return error_string;
}

std::string
ErrorHandler::get_parser_error (const nlc::Parser::ParserError &err) const
{
  const std::string error_reason = get_parser_error_reason (err);
  std::string error_string{};

  if (err.type != nlc::Parser::ParserError::ErrType::PARSER_ERROR_UNEXPECTED)
    {
      error_string += get_message_start (_nol, last_line ().length (), "error",
                                         ESCColor::ESCCOLOR_RED,
                                         ESCGraphics::ESCGRAPHICS_BOLD);
    }
  else
    {
      const auto &token = _tokens.at (err.pos);
      error_string += get_message_start (
          token.line + 1, token.end - token.len + 1, "error",
          ESCColor::ESCCOLOR_RED, ESCGraphics::ESCGRAPHICS_BOLD);
    }
  error_string += error_reason;

  if (err.type == nlc::Parser::ParserError::ErrType::PARSER_ERROR_UNEXPECTED)
    {
      error_string += '\n';
      error_string += get_highlighted_token (_tokens.at (err.pos),
                                             ESCColor::ESCCOLOR_RED,
                                             ESCGraphics::ESCGRAPHICS_BOLD);
    }

  return error_string;
}

std::string
ErrorHandler::get_message_start (size_t line, size_t start,
                                 const std::string &msg, ESCColor color,
                                 ESCGraphics mode) const
{
  std::string out{};

  out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
  out += _filename;
  out += ":" + std::to_string (line) + ":" + std::to_string (start);
  out += ": ";

  out += escape_graphics (mode);
  out += escape_color (color);
  out += msg + ": ";
  out += escape_reset ();

  return out;
}

std::string
ErrorHandler::get_highlighted_token (const nlc::Token &tok, ESCColor color,
                                     ESCGraphics mode) const
{
  constexpr const size_t base_offset = 2;
  constexpr const char *separator = " | ";

  std::string out{};

  const size_t line_num = tok.line + 1;
  const size_t line_num_len = get_number_length (line_num);

  size_t line_num_offset = _numoffset - line_num_len + base_offset;
  for (; line_num_offset > 0; line_num_offset--)
    {
      out += ' ';
    }

  out += std::to_string (line_num);
  out += separator;

  auto line = get_line (tok.line);
  line.insert (tok.end + 1, escape_reset ());
  line.insert (tok.end - tok.len + 1, escape_color (color));
  line.insert (tok.end - tok.len + 1, escape_graphics (mode));

  out += line;
  out += '\n';

  for (size_t i = 0; i < _numoffset + 2; i++)
    {
      out += ' ';
    }
  out += separator;

  const size_t mark_offset = tok.end - tok.len + 1;
  for (size_t i = 0; i < mark_offset; i++)
    {
      out += ' ';
    }

  out += escape_graphics (mode);
  out += escape_color (color);

  out += '^';
  for (size_t i = 0; i < tok.len - 1; i++)
    {
      out += '~';
    }

  out += escape_reset ();

  return out;
}

std::string
ErrorHandler::get_parser_error_reason (
    const nlc::Parser::ParserError &err) const
{
  using namespace nlc;

  switch (err.type)
    {
    case Parser::ParserError::ErrType::PARSER_ERROR_UNK:
      return "<Unknown error>";

    case Parser::ParserError::ErrType::PARSER_ERROR_EXPECTED:
      {
        std::string out{};
        out += "Expected token";
        return out;
      }

    case Parser::ParserError::ErrType::PARSER_ERROR_UNEXPECTED:
      {
        std::string out{};
        out += "Unexpected token \"" + _tokens.at (err.pos).value + "\"";
        return out;
      }

    case Parser::ParserError::ErrType::PARSER_ERROR_INVALID_EXPR:
      {
        std::string out{};
        out += "Invalid expression at line "
               + std::to_string (_tokens.at (err.pos).line + 1);
        return out;
      }

    default:
      return {};
    }
}

bool
ErrorHandler::has_invalid_tokens () const
{
  for (auto &tok : _tokens)
    {
      if (tok.type == nlc::TokenType::ERR)
        {
          return true;
        }
    }
  return false;
}

bool
ErrorHandler::has_parser_errors () const
{
  return _parser_errors.size () > 0;
}

std::string
ErrorHandler::get_line (size_t linenum) const
{
  size_t pos = 0;
  // Skip until found line by number
  while (pos < _src.length () && linenum > 0)
    {
      if (_src.at (pos++) == '\n')
        {
          linenum--;
        }
    }

  std::string buf{};
  while (pos < _src.length ())
    {
      char c = _src.at (pos++);
      if (c == '\n')
        {
          break;
        }
      buf += c;
    }
  return buf;
}

std::string
ErrorHandler::last_line () const
{
  return get_line (_nol - 1);
}

size_t
ErrorHandler::get_number_length (long long num) const
{
  size_t len = 0;
  if (num == 0)
    {
      return 1;
    }
  else if (num < 0)
    {
      len++;
      num *= -1;
    }

  while (num > 0)
    {
      num /= 10;
      len++;
    }

  return len;
}
