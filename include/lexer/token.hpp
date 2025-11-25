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

  LARROW, // Left arrow: <-
  RARROW, // Right arrow: ->

  LTHAN, // Less than symbol: <
  GTHAN, // Greater than symbol: >

  COLON, // Colon: :
  DCOLON, // Double colon: ::

  SEMI, // Semicolon: ;

  COMMA, // Comma: ,
  PERIOD, // Period: .

  BAND, // Binary and: &
  BOR, // Binary or: |
  BXOR, // Binary xor: ^
  BNOT, // Binary not: ~

  AND, // Boolean and: &&
  OR, // Boolean or: ||
  NOT, // Boolean not: !
  QUE, // Question mark: ?

  LTHANEQ, // Less than or equal: <=
  GTHANEQ, // Greater than or equal: >=
  NOTEQ, // Not equal: !=

  ADD, // Addition: +
  SUB, // Subtraction: -
  MUL, // Multiplication: *
  DIV, // Division: /
  MOD, // Module: %
  SHL, // Shift left: <<
  SHR, // Shift right: >>

  EQ, // Assignment: =
  ADDEQ, // Addition assignment: +=
  SUBEQ, // Subtraction assignment: -=
  MULEQ, // Multiplication assignment: *=
  DIVEQ, // Division assignment: /=
  MODEQ, // Module assignment: %=
  BANDEQ, // Binary and assignment: &=
  BOREQ, // Binary or assignment: |=
  BXOREQ, // Binary xor assignment: ^=
  SHLEQ, // Shift left assignment: <<=
  SHREQ, // Shift right assignment: >>=

  EQEQ, // Comparison (double equal): ==

  AT, // At (label marker): @
  HASH, // Hashtag: #
};

struct Token {
  size_t line; // Line at which token has occured
  size_t end; // Absolute position of end of token
  size_t len; // Length of token
  TokenType type; // Token type
  std::string value; // Token value

  Token(size_t line, size_t end, size_t len, TokenType type,
        const std::string &value = "")
    : line{ line }
    , end{ end }
    , len{ len }
    , type{ type }
    , value{ value }
  {
  }
  Token() = default;

  // Turn token into a string, used for debugging.
  std::string to_string() const;
};

}
