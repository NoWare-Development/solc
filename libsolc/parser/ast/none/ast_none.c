#include "containers/string.h"
#include "containers/vector.h"
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

void solc_ast_none_destroy(solc_ast_t *none_ast)
{
  SOLC_ASSUME(none_ast != nullptr && none_ast->type == SOLC_AST_TYPE_NONE_NONE);
  free(none_ast);
}

string_t *solc_ast_none_build_tree(solc_ast_t *none_ast)
{
  SOLC_ASSUME(none_ast != nullptr && none_ast->type == SOLC_AST_TYPE_NONE_NONE);
  string_t *out_v = vector_reserve(string_t, 1);
  vector_push(out_v, string_create_from("NONE"));
  return out_v;
}
