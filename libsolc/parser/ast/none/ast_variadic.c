#include "containers/string.h"
#include "containers/vector.h"
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

string_t *solc_ast_variadic_build_tree(solc_ast_t *variadic_ast)
{
  SOLC_ASSUME(variadic_ast != nullptr &&
              variadic_ast->type == SOLC_AST_TYPE_NONE_VARIADIC);
  string_t *out_v = vector_reserve(string_t, 1);
  vector_push(out_v, string_create_from("VARIADIC"));
  return out_v;
}
