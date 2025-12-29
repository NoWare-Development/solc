#pragma once

#include <cstddef>
#include <string>

namespace solc
{

enum class TokenType {
  // Error token, used to notify that there's
  // a not supported symbol in sources.
  ERR = 0,

  ID, // Identifier: abc

  NUM, // Decimal number: 123
  NUMHEX, // Hexadecimal number: 0xDEADBEEF123
  NUMBIN, // Binary number: 0b10011
  NUMOCT, // Octal number: 0761
  NUMFLOAT, // Floating-point number: 3.14

  STRING, // String in double quotes: "lorem ipsum"
  SYMBOL, // Character in single quotes: 'A'

  LPAREN, // Left (open) parentheses: (
  RPAREN, // Right (close) parentheses: )
  LBRACK, // Left (open) bracket: [
  RBRACK, // Right (close) bracket: ]
  LBRACE, // Left (open) curly brace: {
  RBRACE, // Right (close) curly brace: }

  LTHAN, // Less than symbol: <
  GTHAN, // Greater than symbol: >

  COLON, // Colon: :

  SEMI, // Semicolon: ;

  COMMA, // Comma: ,
  PERIOD, // Period: .

  BAND, // Binary and: &
  BOR, // Binary or: |
  BXOR, // Binary xor: ^
  BNOT, // Binary not: ~

  NOT, // Boolean not: !
  QUE, // Question mark: ?

  ADD, // Addition: +
  SUB, // Subtraction: -
  MUL, // Multiplication: *
  DIV, // Division: /
  MOD, // Module: %

  EQ, // Assignment: =

  AT, // At (label marker): @
  HASH, // Hashtag: #
};

struct Token {
  std::string value; // Token value
  size_t line; // Line at which token has occured
  size_t end; // Absolute position of end of token
  size_t len; // Length of token
  TokenType type; // Token type
  bool has_whitespace_after;

  Token(size_t line, size_t end, size_t len, TokenType type,
        bool has_whitespace_after, const std::string &value = "")
    : value{ value }
    , line{ line }
    , end{ end }
    , len{ len }
    , type{ type }
    , has_whitespace_after(has_whitespace_after)
  {
  }
  Token() = default;

  // Turn token into a string, used for debugging.
  std::string to_string() const;
};

}
