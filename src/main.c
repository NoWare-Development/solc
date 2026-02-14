#include "libsolc/parser/ast/ast_group_expr_operand.h"
#include "libsolc/parser/ast/ast_group_type.h"
#include <solc/lexer/lexer.h>
#include <solc/lexer/token.h>
#include <solc/defs.h>
#include <stdio.h>
#include <stdlib.h>
#include <solc/parser/ast.h>

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

  solc_ast_t *type = solc_ast_type_plain_create(1, "long");
  solc_ast_t *expr = solc_ast_expr_operand_num_create(2, 67, "u");
  solc_ast_t *cast_to = solc_ast_expr_operand_cast_to_create(0, type, expr);
  char *buf = malloc(sizeof(char) * 4096);
  solc_ast_to_string(buf, 4096, cast_to);
  printf("%s", buf);

  free(buf);
  solc_ast_destroy(cast_to);

  return 0;
}
