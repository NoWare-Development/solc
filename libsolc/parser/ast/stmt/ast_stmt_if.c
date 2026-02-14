#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast/ast_group_stmt.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *condition_expr_ast;
  solc_ast_t *stmt_ast;
  solc_ast_t *else_ast;
} ast_if_stmt_t;

solc_ast_t *solc_ast_stmt_if_create(sz pos, solc_ast_t *condition_expr_ast,
                                    solc_ast_t *stmt_ast, solc_ast_t *else_ast)
{
  SOLC_ASSUME(condition_expr_ast != nullptr && stmt_ast != nullptr);
  if (else_ast != nullptr) {
    SOLC_ASSUME(else_ast->type == SOLC_AST_TYPE_STMT_ELSE);
  }
  ast_if_stmt_t *out_if_stmt = malloc(sizeof(ast_if_stmt_t));
  SOLC_AST_INIT_HEADER(out_if_stmt, pos, SOLC_AST_TYPE_STMT_IF);
  out_if_stmt->condition_expr_ast = condition_expr_ast;
  out_if_stmt->stmt_ast = stmt_ast;
  out_if_stmt->else_ast = else_ast;
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
  if (if_data->else_ast != nullptr) {
    SOLC_ASSUME(if_data->else_ast->type == SOLC_AST_TYPE_STMT_ELSE);
    solc_ast_stmt_else_destroy(if_data->else_ast);
  }
  free(if_ast);
}

string_t *solc_ast_stmt_if_build_tree(solc_ast_t *if_ast)
{
  SOLC_ASSUME(if_ast != nullptr && if_ast->type == SOLC_AST_TYPE_STMT_IF);
  SOLC_AST_CAST(if_data, if_ast, ast_if_stmt_t);
  SOLC_ASSUME(if_data->condition_expr_ast != nullptr &&
              if_data->stmt_ast != nullptr);

  string_t header = string_create_from("STMT_IF");
  string_t **children_vs_v = vector_reserve(string_t *, 3);
  vector_push(children_vs_v,
              ast_get_build_tree_func(if_data->condition_expr_ast->type)(
                if_data->condition_expr_ast));
  vector_push(children_vs_v, ast_get_build_tree_func(if_data->stmt_ast->type)(
                               if_data->stmt_ast));
  if (if_data->else_ast != nullptr) {
    SOLC_ASSUME(if_data->else_ast->type == SOLC_AST_TYPE_STMT_ELSE);
    vector_push(children_vs_v,
                solc_ast_stmt_else_build_tree(if_data->else_ast));
  }

  return ast_build_tree(&header, children_vs_v);
}
