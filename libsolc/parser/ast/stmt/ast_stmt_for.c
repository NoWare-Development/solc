#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *init_stmt_ast;
  solc_ast_t *condition_expr_ast;
  solc_ast_t *expr_ast;
  solc_ast_t *stmt_ast;
} ast_for_stmt_t;

solc_ast_t *solc_ast_stmt_for_create(sz pos, solc_ast_t *init_stmt_ast,
                                     solc_ast_t *condition_expr_ast,
                                     solc_ast_t *expr_ast, solc_ast_t *stmt_ast)
{
  SOLC_ASSUME(init_stmt_ast != nullptr && stmt_ast != nullptr);
  switch (init_stmt_ast->type) { // NOTE: it probably needs to include some more
  case SOLC_AST_TYPE_NONE_NONE:
  case SOLC_AST_TYPE_VAR_DECL:
  case SOLC_AST_TYPE_VAR_DEF:
  case SOLC_AST_TYPE_NONE_EXPR:
  case SOLC_AST_TYPE_NONE_PREFIX_EXPR:
    break;
  default:
    SOLC_NOREACH();
  }
  ast_for_stmt_t *out_for_stmt = malloc(sizeof(ast_for_stmt_t));
  SOLC_AST_INIT_HEADER(out_for_stmt, pos, SOLC_AST_TYPE_STMT_FOR);
  out_for_stmt->init_stmt_ast = init_stmt_ast;
  out_for_stmt->condition_expr_ast = condition_expr_ast;
  out_for_stmt->expr_ast = expr_ast;
  out_for_stmt->stmt_ast = stmt_ast;
  return SOLC_AST(out_for_stmt);
}

void solc_ast_stmt_for_destroy(solc_ast_t *for_ast)
{
  SOLC_ASSUME(for_ast != nullptr && for_ast->type == SOLC_AST_TYPE_STMT_FOR);
  SOLC_AST_CAST(for_data, for_ast, ast_for_stmt_t);
  SOLC_ASSUME(for_data->init_stmt_ast != nullptr &&
              for_data->stmt_ast != nullptr);
  solc_ast_destroy(for_data->init_stmt_ast);
  if (for_data->condition_expr_ast != nullptr) {
    solc_ast_destroy(for_data->condition_expr_ast);
  }
  if (for_data->expr_ast != nullptr) {
    solc_ast_destroy(for_data->expr_ast);
  }
  solc_ast_destroy(for_data->stmt_ast);
  free(for_ast);
}

string_t *solc_ast_stmt_for_build_tree(solc_ast_t *for_ast)
{
  SOLC_ASSUME(for_ast != nullptr && for_ast->type == SOLC_AST_TYPE_STMT_FOR);
  SOLC_AST_CAST(for_data, for_ast, ast_for_stmt_t);
  SOLC_ASSUME(for_data->init_stmt_ast != nullptr &&
              for_data->stmt_ast != nullptr);
  string_t header = string_create_from("STMT_FOR");
  string_t **children_vs_v = vector_reserve(string_t *, 4);
  vector_push(children_vs_v,
              ast_get_build_tree_func(for_data->init_stmt_ast->type)(
                for_data->init_stmt_ast));
  if SOLC_LIKELY (for_data->condition_expr_ast != nullptr) {
    vector_push(children_vs_v,
                ast_get_build_tree_func(for_data->condition_expr_ast->type)(
                  for_data->condition_expr_ast));
  }
  if SOLC_LIKELY (for_data->expr_ast != nullptr) {
    vector_push(children_vs_v, ast_get_build_tree_func(
                                 for_data->expr_ast->type)(for_data->expr_ast));
  }
  vector_push(children_vs_v, ast_get_build_tree_func(for_data->stmt_ast->type)(
                               for_data->stmt_ast));

  return ast_build_tree(&header, children_vs_v);
}
