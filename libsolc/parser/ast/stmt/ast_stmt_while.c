#include "containers/string.h"
#include "containers/vector.h"
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
  solc_ast_destroy_if_exists(while_data->condition_expr_ast);
  solc_ast_destroy_if_exists(while_data->stmt_ast);
  free(while_data);
}

string_t *solc_ast_stmt_while_build_tree(solc_ast_t *while_ast)
{
  SOLC_ASSUME(while_ast != nullptr &&
              while_ast->type == SOLC_AST_TYPE_STMT_WHILE);
  SOLC_AST_CAST(while_data, while_ast, ast_while_t);
  string_t header = string_create_from("STMT_WHILE");
  string_t **children_vs_v = vector_reserve(string_t *, 2);
  solc_ast_add_to_tree_if_exists(children_vs_v, while_data->condition_expr_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, while_data->stmt_ast);

  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t *solc_ast_stmt_while_get_condition_expr_ast(solc_ast_t *while_ast)
{
  SOLC_ASSUME(while_ast != nullptr &&
              while_ast->type == SOLC_AST_TYPE_STMT_WHILE);
  SOLC_AST_CAST(while_data, while_ast, ast_while_t);
  return while_data->condition_expr_ast;
}

solc_ast_t *solc_ast_stmt_while_get_stmt_ast(solc_ast_t *while_ast)
{
  SOLC_ASSUME(while_ast != nullptr &&
              while_ast->type == SOLC_AST_TYPE_STMT_WHILE);
  SOLC_AST_CAST(while_data, while_ast, ast_while_t);
  return while_data->stmt_ast;
}
