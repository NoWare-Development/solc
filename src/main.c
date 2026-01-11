#include "solc/lexer/lexer.h"
#include "solc/lexer/token.h"
#include <solc/defs.h>
#include <stdio.h>
#include <stdlib.h>

s32 main(s32 argc, char **argv)
{
  if (argc < 2) {
    return -1;
  }

  FILE *f = fopen(argv[1], "r");
  fseek(f, 0, SEEK_END);
  s32 size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *src = calloc(sizeof(char), size);
  fread(src, sizeof(char), size, f);
  fclose(f);

  solc_lexer_t lexer = solc_lexer_create(src);
  sz tokens_num;
  solc_token_t *tokens = solc_lexer_tokenize(&lexer, &tokens_num);
  for (sz i = 0; i < tokens_num; i++) {
    char buf[0x80] = { 0 };
    solc_token_to_string(buf, sizeof(buf) - 1, &tokens[i]);
    printf("(%zu) %s\n", i, buf);
    // TODO: remove it later
    solc_token_destroy(&tokens[i]);
  }

  free(tokens);

  solc_lexer_destroy(&lexer);

  free(src);

  return 0;
}
