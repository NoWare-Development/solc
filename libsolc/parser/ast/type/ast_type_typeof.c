#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
} ast_typeof_type_t;

solc_ast_t *solc_ast_type_typeof_create(sz pos, solc_ast_t *expr_ast)
{
  SOLC_ASSUME(expr_ast != nullptr);

  ast_typeof_type_t *out_typeof_type = malloc(sizeof(ast_typeof_type_t));
  SOLC_AST_INIT_HEADER(out_typeof_type, pos, SOLC_AST_TYPE_TYPE_TYPEOF);
  out_typeof_type->expr_ast = expr_ast;
  return SOLC_AST(out_typeof_type);
}

void solc_ast_type_typeof_destroy(solc_ast_t *typeof_type_ast)
{
  SOLC_ASSUME(typeof_type_ast != nullptr &&
              typeof_type_ast->type == SOLC_AST_TYPE_TYPE_TYPEOF);
  SOLC_AST_CAST(typeof_type_data, typeof_type_ast, ast_typeof_type_t);
  SOLC_ASSUME(typeof_type_data->expr_ast != nullptr);
  solc_ast_destroy(typeof_type_data->expr_ast);
  free(typeof_type_ast);
}

sz solc_ast_type_typeof_to_string(char *buf, sz n, solc_ast_t *typeof_type_ast)
{
  SOLC_ASSUME(buf != nullptr && typeof_type_ast != nullptr &&
              typeof_type_ast->type == SOLC_AST_TYPE_TYPE_TYPEOF);

  SOLC_TODO("Type typeof to string.");

  return 0;
}
