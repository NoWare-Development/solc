#include "containers/string.h"
#include "containers/vector.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

solc_ast_t *solc_ast_stmt_fallthrough_create(sz pos)
{
  solc_ast_t *out_fallthrough_stmt = malloc(sizeof(solc_ast_t));
  out_fallthrough_stmt->token_pos = pos;
  out_fallthrough_stmt->type = SOLC_AST_TYPE_STMT_FALLTHROUGH;
  return out_fallthrough_stmt;
}

void solc_ast_stmt_fallthrough_destroy(solc_ast_t *fallthrough_ast)
{
  SOLC_ASSUME(fallthrough_ast != nullptr &&
              fallthrough_ast->type == SOLC_AST_TYPE_STMT_FALLTHROUGH);
  free(fallthrough_ast);
}

string_t *solc_ast_stmt_fallthrough_build_tree(solc_ast_t *fallthrough_ast)
{
  SOLC_ASSUME(fallthrough_ast != nullptr &&
              fallthrough_ast->type == SOLC_AST_TYPE_STMT_FALLTHROUGH);
  string_t *out_v = vector_reserve(string_t, 1);
  vector_push(out_v, string_create_from("STMT_FALLTHROUGH"));
  return out_v;
}
