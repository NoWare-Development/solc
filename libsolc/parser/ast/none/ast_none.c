#include "parser/ast_private.h"
#include "solc/defs.h"
#include <stdlib.h>

solc_ast_t *solc_ast_none_create(sz pos)
{
  solc_ast_t *out_none = (solc_ast_t *)malloc(sizeof(solc_ast_t));
  out_none->token_pos = pos;
  out_none->type = SOLC_AST_TYPE_NONE_NONE;
  return out_none;
}

void solc_ast_none_destory(solc_ast_t *none_ast)
{
  SOLC_ASSUME(none_ast != nullptr && none_ast->type == SOLC_AST_TYPE_NONE_NONE);
  free(none_ast);
}

sz solc_ast_none_to_string(char *buf, sz n, solc_ast_t *none_ast)
{
  SOLC_ASSUME(buf != nullptr && none_ast != nullptr &&
              none_ast->type == SOLC_AST_TYPE_NONE_NONE);
  return snprintf(buf, n, "NONE");
}
