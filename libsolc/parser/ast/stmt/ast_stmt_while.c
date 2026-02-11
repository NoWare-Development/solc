#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *condition_expr_ast;
  solc_ast_t *stmt_ast;
} ast_while_t;

solc_ast_t *solc_ast_stmt_while_create(sz pos, solc_ast_t *condition_expr_ast,
                                       solc_ast_t *stmt_ast)
{
  SOLC_ASSUME(condition_expr_ast != nullptr && stmt_ast != nullptr);
  ast_while_t *out_while_stmt = malloc(sizeof(ast_while_t));
  SOLC_AST_INIT_HEADER(out_while_stmt, pos, SOLC_AST_TYPE_STMT_WHILE);
  out_while_stmt->condition_expr_ast = condition_expr_ast;
  out_while_stmt->stmt_ast = stmt_ast;
  return SOLC_AST(out_while_stmt);
}

void solc_ast_stmt_while_destroy(solc_ast_t *while_ast)
{
  SOLC_ASSUME(while_ast != nullptr &&
              while_ast->type == SOLC_AST_TYPE_STMT_WHILE);
  SOLC_AST_CAST(while_data, while_ast, ast_while_t);
  SOLC_ASSUME(while_data->condition_expr_ast != nullptr &&
              while_data->stmt_ast != nullptr);
  solc_ast_destroy(while_data->condition_expr_ast);
  solc_ast_destroy(while_data->stmt_ast);
  free(while_data);
}

sz solc_ast_stmt_while_to_string(char *buf, sz n, solc_ast_t *while_ast)
{
  SOLC_ASSUME(buf != nullptr && while_ast != nullptr &&
              while_ast->type == SOLC_AST_TYPE_STMT_WHILE);

  SOLC_TODO("While statement to string.");

  return 0;
}
