#ifndef __SOLC_LEXER_H__
#define __SOLC_LEXER_H__

#include <solc/defs.h>
#include <solc/lexer/token.h>

typedef struct {
  solc_token_t *tokens_v;
  const char *src;
  sz src_len;
  sz pos;
  sz line;
  sz llp;
} solc_lexer_t;

__SOLC_CPP_GUARD_TOP()

solc_lexer_t *solc_lexer_create(const char *src);
void solc_lexer_destroy(solc_lexer_t *lexer);

// Returned array must be freed with `free()' later.
solc_token_t *solc_lexer_tokenize(solc_lexer_t *lexer, sz *out_token_num);

__SOLC_CPP_GUARD_BOTTOM()

#endif // __SOLC_LEXER_H__
