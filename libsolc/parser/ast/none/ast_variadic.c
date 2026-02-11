#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

solc_ast_t *solc_ast_variadic_create(sz pos)
{
  solc_ast_t *out_variadic = (solc_ast_t *)malloc(sizeof(solc_ast_t));
  out_variadic->token_pos = pos;
  out_variadic->type = SOLC_AST_TYPE_NONE_VARIADIC;
  return out_variadic;
}

void solc_ast_variadic_destroy(solc_ast_t *variadic_ast)
{
  SOLC_ASSUME(variadic_ast != nullptr &&
              variadic_ast->type == SOLC_AST_TYPE_NONE_VARIADIC);
  free(variadic_ast);
}

sz solc_ast_variadic_to_string(char *buf, sz n, solc_ast_t *variadic_ast)
{
  SOLC_ASSUME(buf != nullptr && variadic_ast != nullptr &&
              variadic_ast->type == SOLC_AST_TYPE_NONE_VARIADIC);
  return snprintf(buf, n, "VARIADIC");
}
