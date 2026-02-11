#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *stmt_ast;
} ast_else_t;

solc_ast_t *solc_ast_stmt_else_create(sz pos, solc_ast_t *stmt_ast)
{
  SOLC_ASSUME(stmt_ast != nullptr);
  ast_else_t *out_else_stmt = malloc(sizeof(ast_else_t));
  SOLC_AST_INIT_HEADER(out_else_stmt, pos, SOLC_AST_TYPE_STMT_ELSE);
  out_else_stmt->stmt_ast = stmt_ast;
  return SOLC_AST(out_else_stmt);
}

void solc_ast_stmt_else_destroy(solc_ast_t *else_ast)
{
  SOLC_ASSUME(else_ast != nullptr && else_ast->type == SOLC_AST_TYPE_STMT_ELSE);
  SOLC_AST_CAST(else_data, else_ast, ast_else_t);
  SOLC_ASSUME(else_data->stmt_ast != nullptr);
  solc_ast_destroy(else_data->stmt_ast);
  free(else_ast);
}

sz solc_ast_stmt_else_to_string(char *buf, sz n, solc_ast_t *else_ast)
{
  SOLC_ASSUME(buf != nullptr && else_ast != nullptr &&
              else_ast->type == SOLC_AST_TYPE_STMT_ELSE);

  SOLC_TODO("Else statement to string.");

  return 0;
}
