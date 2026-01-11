#ifndef __SOLC_LEXER_H__
#define __SOLC_LEXER_H__

#include <solc/defs.h>
#include <solc/lexer/token.h>

typedef struct {
  void *data;
} solc_lexer_t;

solc_lexer_t solc_lexer_create(const char *src);
void solc_lexer_destroy(solc_lexer_t *lexer);

// Returned array must be freed with `free()' later.
solc_token_t *solc_lexer_tokenize(solc_lexer_t *lexer, sz *out_token_num);

#endif // __SOLC_LEXER_H__
