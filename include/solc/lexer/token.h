#ifndef __SOLC_TOKEN_H__
#define __SOLC_TOKEN_H__

#include <solc/defs.h>

typedef enum {
  // Error token, used to notify that there's
  // a not supported symbol in source file.
  SOLC_TOKENTYPE_ERR = 0,

  SOLC_TOKENTYPE_ID = 1, // Identifier: abc xyz mytest_ _supertest$2

  SOLC_TOKENTYPE_NUM = 2, // Decimal number: 123
  SOLC_TOKENTYPE_NUMHEX = 3, // Hexadecimal number: 0xDEADBEEF123 0xdeadbeef123
  SOLC_TOKENTYPE_NUMBIN = 4, // Binary number: 0b10011
  SOLC_TOKENTYPE_NUMOCT = 5, // Octal number: 0761
  SOLC_TOKENTYPE_NUMFLOAT = 6, // Floating-point number: 3.14

  SOLC_TOKENTYPE_STRING = 7, // String in double quotes: "lorem ipsum"
  SOLC_TOKENTYPE_SYMBOL = 8, // Character in single quotes: 'A' '\n'

  SOLC_TOKENTYPE_LPAREN = 9, // Left (open) parantheses: (
  SOLC_TOKENTYPE_RPAREN = 10, // Right (close) parantheses: )
  SOLC_TOKENTYPE_LBRACK = 11, // Left (open) bracket: [
  SOLC_TOKENTYPE_RBRACK = 12, // Right (close) bracket: ]
  SOLC_TOKENTYPE_LCBRACK = 13, // Left (open) curly bracket: {
  SOLC_TOKENTYPE_RCBRACK = 14, // Right (close) curly bracket: }

  SOLC_TOKENTYPE_LARROW = 15, // Less than symbol (left arrow head): <
  SOLC_TOKENTYPE_RARROW = 16, // Greater than symbol (right arrow head): >

  SOLC_TOKENTYPE_COLON = 17, // Colon: :
  SOLC_TOKENTYPE_SEMI = 18, // Semicolon: ;

  SOLC_TOKENTYPE_COMMA = 19, // Comma: ,
  SOLC_TOKENTYPE_PERIOD = 20, // Period: .

  SOLC_TOKENTYPE_AMPERSAND = 21, // Ampersand: &
  SOLC_TOKENTYPE_PIPE = 22, // Pipe: |
  SOLC_TOKENTYPE_CIRCUMFLEX = 23, // Curcumflex accent: ^
  SOLC_TOKENTYPE_TILDE = 24, // Tilde: ~

  SOLC_TOKENTYPE_EXCLMARK = 25, // Exclamation mark: !

  SOLC_TOKENTYPE_PLUS = 26, // Plus: +
  SOLC_TOKENTYPE_MINUS = 27, // Minus: -
  SOLC_TOKENTYPE_ASTERISK = 28, // Asterisk: *
  SOLC_TOKENTYPE_SLASH = 29, // Slash: /
  SOLC_TOKENTYPE_PERCENT = 30, // Percent: %

  SOLC_TOKENTYPE_EQ = 31, // Equal sign: =

  SOLC_TOKENTYPE_AT = 32, // Commercial at: @
  SOLC_TOKENTYPE_HASH = 33, // Number sign (hash): #

  SOLC_TOKENTYPE_MAX,
} solc_tokentype_t;

typedef struct __solc_token_t {
  char *value;
  sz line;
  sz end;
  sz len;
  solc_tokentype_t type;
  b8 has_whitespace_after;
} solc_token_t;

void solc_token_destroy(solc_token_t *token);

void solc_token_to_string(char *buf, sz n, const solc_token_t *token);

#endif // __SOLC_TOKEN_H__
