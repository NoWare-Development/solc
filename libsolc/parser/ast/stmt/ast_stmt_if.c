#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *condition_expr_ast;
  solc_ast_t *stmt_ast;
} ast_if_stmt_t;

solc_ast_t *solc_ast_stmt_if_create(sz pos, solc_ast_t *condition_expr_ast,
                                    solc_ast_t *stmt_ast)
{
  SOLC_ASSUME(condition_expr_ast != nullptr && stmt_ast != nullptr);
  ast_if_stmt_t *out_if_stmt = malloc(sizeof(ast_if_stmt_t));
  SOLC_AST_INIT_HEADER(out_if_stmt, pos, SOLC_AST_TYPE_STMT_IF);
  out_if_stmt->condition_expr_ast = condition_expr_ast;
  out_if_stmt->stmt_ast = stmt_ast;
  return SOLC_AST(out_if_stmt);
}

void solc_ast_stmt_if_destroy(solc_ast_t *if_ast)
{
  SOLC_ASSUME(if_ast != nullptr && if_ast->type == SOLC_AST_TYPE_STMT_IF);
  SOLC_AST_CAST(if_data, if_ast, ast_if_stmt_t);
  SOLC_ASSUME(if_data->condition_expr_ast != nullptr &&
              if_data->stmt_ast != nullptr);
  solc_ast_destroy(if_data->condition_expr_ast);
  solc_ast_destroy(if_data->stmt_ast);
  free(if_ast);
}

sz solc_ast_stmt_if_to_string(char *buf, sz n, solc_ast_t *if_ast)
{
  SOLC_ASSUME(buf != nullptr && if_ast != nullptr &&
              if_ast->type == SOLC_AST_TYPE_STMT_IF);

  SOLC_TODO("If statement to string.");

  return 0;
}
