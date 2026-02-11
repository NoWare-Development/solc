#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *condition_expr_ast;
  solc_ast_t *stmt_ast;
} ast_dowhile_t;

solc_ast_t *solc_ast_stmt_dowhile_create(sz pos, solc_ast_t *condition_expr_ast,
                                         solc_ast_t *stmt_ast)
{
  SOLC_ASSUME(condition_expr_ast != nullptr && stmt_ast != nullptr);
  ast_dowhile_t *out_dowhile_ast = malloc(sizeof(ast_dowhile_t));
  SOLC_AST_INIT_HEADER(out_dowhile_ast, pos, SOLC_AST_TYPE_STMT_DOWHILE);
  out_dowhile_ast->condition_expr_ast = condition_expr_ast;
  out_dowhile_ast->stmt_ast = stmt_ast;
  return SOLC_AST(out_dowhile_ast);
}

void solc_ast_stmt_dowhile_destroy(solc_ast_t *dowhile_ast)
{
  SOLC_ASSUME(dowhile_ast != nullptr &&
              dowhile_ast->type == SOLC_AST_TYPE_STMT_DOWHILE);
  SOLC_AST_CAST(dowhile_data, dowhile_ast, ast_dowhile_t);
  SOLC_ASSUME(dowhile_data->condition_expr_ast != nullptr &&
              dowhile_data->stmt_ast != nullptr);
  solc_ast_destroy(dowhile_data->condition_expr_ast);
  solc_ast_destroy(dowhile_data->stmt_ast);
  free(dowhile_data);
}

sz solc_ast_stmt_dowhile_to_string(char *buf, sz n, solc_ast_t *dowhile_ast)
{
  SOLC_ASSUME(buf != nullptr && dowhile_ast != nullptr &&
              dowhile_ast->type == SOLC_AST_TYPE_STMT_DOWHILE);

  SOLC_TODO("Do-while statement to string.");

  return 0;
}
