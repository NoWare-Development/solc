#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
} ast_expr_stmt_t;

solc_ast_t *solc_ast_stmt_expr_create(sz pos, solc_ast_t *expr_ast)
{
  ast_expr_stmt_t *out_expr_stmt_ast = malloc(sizeof(ast_expr_stmt_t));
  SOLC_AST_INIT_HEADER(out_expr_stmt_ast, pos, SOLC_AST_TYPE_STMT_EXPR);
  out_expr_stmt_ast->expr_ast = expr_ast;
  return SOLC_AST(out_expr_stmt_ast);
}

void solc_ast_stmt_expr_destroy(solc_ast_t *expr_stmt_ast)
{
  SOLC_ASSUME(expr_stmt_ast != nullptr &&
              expr_stmt_ast->type == SOLC_AST_TYPE_STMT_EXPR);
  SOLC_AST_CAST(expr_stmt_data, expr_stmt_ast, ast_expr_stmt_t);
  solc_ast_destroy_if_exists(expr_stmt_data->expr_ast);
  free(expr_stmt_data);
}

string_t *solc_ast_stmt_expr_build_tree(solc_ast_t *expr_stmt_ast)
{
  SOLC_ASSUME(expr_stmt_ast != nullptr &&
              expr_stmt_ast->type == SOLC_AST_TYPE_STMT_EXPR);
  SOLC_AST_CAST(expr_stmt_data, expr_stmt_ast, ast_expr_stmt_t);
  string_t header = string_create_from("STMT_EXPR");
  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v, expr_stmt_data->expr_ast);

  return ast_build_tree(&header, children_vs_v);
}
