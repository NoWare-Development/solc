#include "lexer/lexer.hpp"
#include <cctype>
#include <iostream>
#include <vector>

namespace nlc
{

std::vector<Token>
Lexer::tokenize (const std::string &src)
{
  std::vector<Token> out{};
  _pos = 0;
  _src = src;
  _line = 0;
  _llp = 0;

  while (_pos < _src.length ())
    {
      char c = _src.at (_pos);

      if (isspace (c))
        {
          if (c == '\n')
            {
              _line++;
              _llp = _pos + 1;
            }
          _pos++;
          continue;
        }

      if (!is_processable (c))
        {
          out.push_back (process_err ());
          continue;
        }

      if (is_start_of_id (c))
        {
          out.push_back (process_id ());
          continue;
        }

      if (isdigit (c))
        {
          out.push_back (process_num ());
          continue;
        }

      switch (c)
        {
        case '"':
          {
            out.push_back (process_string ());
            continue;
          }

        case '\'':
          {
            out.push_back (process_symbol ());
            continue;
          }

        case '(':
          {
            out.push_back (gen_token (1, _pos, TokenType::LPAREN, "("));
            break;
          }
        case ')':
          {
            out.push_back (gen_token (1, _pos, TokenType::RPAREN, ")"));
            break;
          }

        case '[':
          {
            out.push_back (gen_token (1, _pos, TokenType::LBRACK, "["));
            break;
          }
        case ']':
          {
            out.push_back (gen_token (1, _pos, TokenType::RBRACK, "]"));
            break;
          }

        case '{':
          {
            out.push_back (gen_token (1, _pos, TokenType::LBRACE, "{"));
            break;
          }
        case '}':
          {
            out.push_back (gen_token (1, _pos, TokenType::RBRACE, "}"));
            break;
          }

        case '<':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '=')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::LTHANEQ, "<="));
                _pos += 2;
                continue;
              }
            else if (n1 == '<')
              {
                char n2 = peek (_pos + 2);
                if (n2 == '=')
                  {
                    out.push_back (
                        gen_token (3, _pos + 2, TokenType::SHLEQ, "<<="));
                    _pos += 3;
                    continue;
                  }
                out.push_back (gen_token (2, _pos + 1, TokenType::SHL, "<<"));
                _pos += 2;
                continue;
              }
            else if (n1 == '-')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::LARROW, "<-"));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::LTHAN, "<"));
            break;
          }
        case '>':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '=')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::GTHANEQ, ">="));
                _pos += 2;
                continue;
              }
            else if (n1 == '>')
              {
                char n2 = peek (_pos + 2);
                if (n2 == '=')
                  {
                    out.push_back (
                        gen_token (3, _pos + 2, TokenType::SHREQ, ">>="));
                    _pos += 3;
                    continue;
                  }
                out.push_back (gen_token (2, _pos + 1, TokenType::SHR, ">>"));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::GTHAN, ">"));
            break;
          }

        case '+':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '=')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::ADDEQ, "+="));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::ADD, "+"));
            break;
          }
        case '-':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '>')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::RARROW, "->"));
                _pos += 2;
                continue;
              }
            else if (n1 == '=')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::SUBEQ, "-="));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::SUB, "-"));
            break;
          }
        case '*':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '=')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::MULEQ, "*="));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::MUL, "*"));
            break;
          }
        case '/':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '/' || n1 == '*')
              {
                skip_comments ();
                continue;
              }
            else if (n1 == '=')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::DIVEQ, "/="));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::DIV, "/"));
            break;
          }
        case '%':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '=')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::MODEQ, "%="));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::MOD, "%"));
            break;
          }

        case '&':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '&')
              {
                out.push_back (gen_token (2, _pos + 1, TokenType::AND, "&&"));
                _pos += 2;
                continue;
              }
            else if (n1 == '=')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::BANDEQ, "&="));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::BAND, "&"));
            break;
          }
        case '|':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '|')
              {
                out.push_back (gen_token (2, _pos + 1, TokenType::OR, "||"));
                _pos += 2;
                continue;
              }
            else if (n1 == '=')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::BOREQ, "|="));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::BOR, "|"));
            break;
          }
        case '~':
          {
            out.push_back (gen_token (1, _pos, TokenType::BNOT, "~"));
            break;
          }
        case '^':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '=')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::BXOREQ, "^="));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::BXOR, "^"));
            break;
          }

        case '=':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '=')
              {
                out.push_back (gen_token (2, _pos + 1, TokenType::EQEQ, "=="));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::EQ, "="));
            break;
          }

        case '!':
          {
            char n1 = peek (_pos + 1);
            if (n1 == '=')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::NOTEQ, "!="));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::NOT, "!"));
            break;
          }

        case '@':
          {
            out.push_back (gen_token (1, _pos, TokenType::AT, "@"));
            break;
          }

        case ':':
          {
            char n1 = peek (_pos + 1);
            if (n1 == ':')
              {
                out.push_back (
                    gen_token (2, _pos + 1, TokenType::DCOLON, "::"));
                _pos += 2;
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::COLON, ":"));
            break;
          }

        case ';':
          {
            out.push_back (gen_token (1, _pos, TokenType::SEMI, ";"));
            break;
          }

        case '#':
          {
            out.push_back (gen_token (1, _pos, TokenType::HASH, "#"));
            break;
          }

        case '.':
          {
            if (isdigit (peek (_pos + 1)))
              {
                out.push_back (process_num ());
                continue;
              }
            out.push_back (gen_token (1, _pos, TokenType::PERIOD, "."));
            break;
          }

        case ',':
          {
            out.push_back (gen_token (1, _pos, TokenType::COMMA, ","));
            break;
          }

        case '?':
          {
            out.push_back (gen_token (1, _pos, TokenType::QUE, "?"));
            break;
          }

        default:
          break;
        }

      _pos++;
    }

  return out;
}

Token
Lexer::process_id ()
{
  std::string valbuf{};
  valbuf += _src.at (_pos++);
  while (_pos < _src.length ())
    {
      char c = _src.at (_pos);
      if (!is_char_of_id (c))
        {
          break;
        }
      _pos++;
      valbuf += c;
    }
  return gen_token (valbuf.length (), _pos - 1, TokenType::ID, valbuf);
}

Token
Lexer::process_num ()
{
  char c = _src.at (_pos);
  if (c == '0')
    {
      char c2 = _src.at (_pos + 1);
      switch (c2)
        {
        case 'x':
          return process_numhex ();
        case 'b':
          return process_numoct ();
        default:
          if (isdigit (c2))
            {
              return process_numoct ();
            }
          else if (c2 != '.')
            {
              _pos++;
              return gen_token (1, _pos - 1, TokenType::NUM, "0");
            }
          break;
        }
    }

  std::string buf{};
  bool has_dot = false;
  while (_pos < _src.length ())
    {
      char c = _src.at (_pos);
      if (!isdigit (c) && c != '.' && c != '\'' && c != '_')
        {
          break;
        }
      if (c == '\'' || c == '_')
        {
          _pos++;
          continue;
        }

      if (c == '.')
        {
          if (has_dot)
            {
              break;
            }
          has_dot = true;
        }
      buf += c;
      _pos++;
    }

  std::cout << buf << '\n';

  return gen_token (buf.length (), _pos - 1,
                    has_dot ? TokenType::NUMFLOAT : TokenType::NUM, buf);
}

Token
Lexer::process_numhex ()
{
  _pos += 2;
  std::string buf{};
  while (_pos < _src.length ())
    {
      char c = _src.at (_pos);
      if (!isxdigit (c))
        {
          break;
        }
      buf += c;
      _pos++;
    }
  return gen_token (buf.length () + 1, _pos - 1, TokenType::NUMHEX, buf);
}

Token
Lexer::process_numbin ()
{
  _pos += 2;
  std::string buf{};
  while (_pos < _src.length ())
    {
      char c = _src.at (_pos);
      if (c != '1' && c != '0')
        {
          break;
        }
      buf += c;
      _pos++;
    }
  return gen_token (buf.length () + 2, _pos - 1, TokenType::NUMBIN, buf);
}

Token
Lexer::process_numoct ()
{
  _pos += 1;
  std::string buf{};
  while (_pos < _src.length ())
    {
      char c = _src.at (_pos);
      if (c < '0' || c > '7')
        {
          break;
        }
      buf += c;
      _pos++;
    }
  return gen_token (buf.length () + 1, _pos - 1, TokenType::NUMOCT, buf);
}

Token
Lexer::process_string ()
{
  _pos += 1;
  std::string buf{};
  size_t rawlen = 1;
  while (_pos < _src.length ())
    {
      char c = _src.at (_pos);

      if (c == '\"')
        {
          _pos++;
          rawlen++;
          break;
        }

      buf += c;
      _pos++;
      rawlen++;
    }
  return gen_token (rawlen, _pos - 1, TokenType::STRING, buf);
}

Token
Lexer::process_symbol ()
{
  _pos += 1;
  std::string buf{};
  size_t rawlen = 1;
  while (_pos < _src.length ())
    {
      char c = _src.at (_pos);
      if (c == '\'')
        {
          rawlen++;
          _pos++;
          break;
        }

      buf += c;
      _pos++;
      rawlen++;
    }
  return gen_token (rawlen, _pos - 1, TokenType::SYMBOL, buf);
}

Token
Lexer::process_err ()
{
  std::string errbuf{};
  size_t start = _pos;
  while (_pos < _src.length ())
    {
      char c = _src.at (_pos);
      if (is_processable (c) || isspace (c))
        {
          break;
        }
      _pos++;
      errbuf += c;
    }
  return gen_token (_pos - start, _pos - 1, TokenType::ERR, errbuf);
}

void
Lexer::skip_comments ()
{
  if (_pos + 1 >= _src.length ())
    return;

  char c1 = _src.at (_pos);
  char c2 = _src.at (_pos + 1);
  if (c1 == c2 && c2 == '/') // c++ comments
    {
      while (_pos < _src.length ())
        {
          char c = _src.at (_pos);
          if (c == '\n')
            {
              _line++;
              _pos++;
              _llp = _pos;
              break;
            }
          _pos++;
        }
    }
  else if (c1 == '/' && c2 == '*') // c comments
    {
      _pos += 2;
      while (_pos < _src.length ())
        {
          char c = _src.at (_pos);
          if (c == '\n')
            {
              _line++;
              _pos++;
              _llp = _pos;
              continue;
            }

          char c2 = _src.at (_pos + 1);
          if (c == '*' && c2 == '/')
            {
              _pos += 2;
              break;
            }

          _pos++;
        }
    }
}

bool
Lexer::is_start_of_id (char c) const
{
  return isalpha (c) || c == '$' || c == '_';
}

bool
Lexer::is_char_of_id (char c) const
{
  return is_start_of_id (c) || isdigit (c);
}

Token
Lexer::gen_token (size_t len, size_t end, TokenType type,
                  const std::string &value) const
{
  return Token (_line, end - _llp, len, type, value);
}

char
Lexer::peek (size_t pos) const
{
  if (pos < _src.length ())
    {
      return _src.at (pos);
    }
  return 0;
}

bool
Lexer::is_processable (char c) const
{
  // This should probably be done in other way but... idk how to
  return is_start_of_id (c) || isdigit (c) || c == '.' || c == '!' || c == '?'
         || c == '*' || c == '/' || c == '+' || c == '-' || c == '^'
         || c == '&' || c == '|' || c == '(' || c == ')' || c == '['
         || c == ']' || c == '{' || c == '}' || c == '"' || c == '\''
         || c == '<' || c == '>' || c == '=' || c == '%' || c == '@'
         || c == ':' || c == ';' || c == '#' || c == ',' || c == '~';
}

}
