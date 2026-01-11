#include <stdio.h>
#include <solc/lexer/token.h>
#include <stdlib.h>
#include <string.h>

static inline const char *tokentype_to_string(solc_tokentype_t type);

void solc_token_destroy(solc_token_t *token)
{
  if (token->value != nullptr) {
    free(token->value);
  }
}

void solc_token_to_string(char *buf, sz n, const solc_token_t *token)
{
  static const char *__true_str = "true";
  static const char *__false_str = "false";

  if (token->value != nullptr && strlen(token->value) > 0) {
    snprintf(buf, n,
             "Token { type: %s, value: \"%s\", has_whitespace_after: %s }",
             tokentype_to_string(token->type), token->value,
             token->has_whitespace_after ? __true_str : __false_str);
  } else {
    snprintf(buf, n, "Token { type: %s, has_whitespace_after: %s }",
             tokentype_to_string(token->type),
             token->has_whitespace_after ? __true_str : __false_str);
  }
}

static const char *__tokentype_strs[SOLC_TOKENTYPE_MAX] = {
  "ERR",      "ID",       "NUM",     "NUMHEX",    "NUMBIN",   "NUMOCT",
  "NUMFLOAT", "STRING",   "SYMBOL",  "LPAREN",    "RPAREN",   "LBRACK",
  "RBRACK",   "LCBRACK",  "RCBRACK", "LARROW",    "RARROW",   "COLON",
  "SEMI",     "COMMA",    "PERIOD",  "AMPERSAND", "PIPE",     "CIRCUMFLEX",
  "TILDE",    "EXCLMARK", "PLUS",    "MINUS",     "ASTERISK", "SLASH",
  "PERCENT",  "EQ",       "AT",      "HASH",
};

static inline const char *tokentype_to_string(solc_tokentype_t type)
{
  return __tokentype_strs[type];
}
