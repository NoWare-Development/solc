#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *stmt_ast;
} ast_defer_stmt_t;

solc_ast_t *solc_ast_stmt_defer_create(sz pos, solc_ast_t *stmt_ast)
{
  SOLC_ASSUME(stmt_ast != nullptr);
  ast_defer_stmt_t *out_defer_stmt = malloc(sizeof(ast_defer_stmt_t));
  SOLC_AST_INIT_HEADER(out_defer_stmt, pos, SOLC_AST_TYPE_STMT_DEFER);
  out_defer_stmt->stmt_ast = stmt_ast;
  return SOLC_AST(out_defer_stmt);
}

void solc_ast_stmt_defer_destroy(solc_ast_t *defer_ast)
{
  SOLC_ASSUME(defer_ast != nullptr &&
              defer_ast->type == SOLC_AST_TYPE_STMT_DEFER);
  SOLC_AST_CAST(defer_data, defer_ast, ast_defer_stmt_t);
  SOLC_ASSUME(defer_data->stmt_ast != nullptr);
  solc_ast_destroy(defer_data->stmt_ast);
  free(defer_data);
}

sz solc_ast_stmt_defer_to_string(char *buf, sz n, solc_ast_t *defer_ast)
{
  SOLC_ASSUME(buf != nullptr && defer_ast != nullptr &&
              defer_ast->type == SOLC_AST_TYPE_STMT_DEFER);

  SOLC_TODO("Defer statement to string.");

  return 0;
}
