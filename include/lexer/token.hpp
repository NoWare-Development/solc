#pragma once

#include <cstddef>
#include <string>

namespace solc
{

enum class TokenType {
  ERR = 0,

  ID,

  NUM,
  NUMHEX,
  NUMBIN,
  NUMOCT,
  NUMFLOAT,

  STRING,
  SYMBOL,

  LPAREN,
  RPAREN,
  LBRACK,
  RBRACK,
  LBRACE,
  RBRACE,

  LARROW,
  RARROW,

  LTHAN,
  GTHAN,

  COLON,
  DCOLON,

  SEMI,

  COMMA,
  PERIOD,

  BAND,
  BOR,
  BXOR,
  BNOT,

  AND,
  OR,
  NOT,
  QUE,

  LTHANEQ,
  GTHANEQ,
  NOTEQ,

  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  SHL,
  SHR,

  EQ,
  ADDEQ,
  SUBEQ,
  MULEQ,
  DIVEQ,
  MODEQ,
  BANDEQ,
  BOREQ,
  BXOREQ,
  SHLEQ,
  SHREQ,

  EQEQ,

  AT,
  HASH,
};

struct Token {
  size_t line;
  size_t end;
  size_t len;
  TokenType type;
  std::string value;

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

  std::string to_string() const;
};

}
