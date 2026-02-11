#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
} ast_return_stmt_t;

solc_ast_t *solc_ast_stmt_return_create(sz pos, solc_ast_t *expr_ast)
{
  ast_return_stmt_t *out_return_stmt = malloc(sizeof(ast_return_stmt_t));
  SOLC_AST_INIT_HEADER(out_return_stmt, pos, SOLC_AST_TYPE_STMT_RETURN);
  out_return_stmt->expr_ast = expr_ast;
  return SOLC_AST(out_return_stmt);
}

void solc_ast_stmt_return_destroy(solc_ast_t *return_ast)
{
  SOLC_ASSUME(return_ast != nullptr &&
              return_ast->type == SOLC_AST_TYPE_STMT_RETURN);
  SOLC_AST_CAST(return_data, return_ast, ast_return_stmt_t);
  if (return_data->expr_ast != nullptr) {
    solc_ast_destroy(return_data->expr_ast);
  }
  free(return_ast);
}

sz solc_ast_stmt_return_to_string(char *buf, sz n, solc_ast_t *return_ast)
{
  SOLC_ASSUME(buf != nullptr && return_ast != nullptr &&
              return_ast->type == SOLC_AST_TYPE_STMT_RETURN);

  SOLC_TODO("Return statement to string.");

  return 0;
}
