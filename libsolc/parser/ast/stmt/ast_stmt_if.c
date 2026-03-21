#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast/ast_group_stmt.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *condition_expr_ast;
  solc_ast_t *attrib_list_ast;
  solc_ast_t *stmt_ast;
  solc_ast_t *else_ast;
} ast_if_stmt_t;

solc_ast_t *solc_ast_stmt_if_create(sz pos, solc_ast_t *condition_expr_ast,
                                    solc_ast_t *attrib_list_ast,
                                    solc_ast_t *stmt_ast, solc_ast_t *else_ast)
{
  ast_if_stmt_t *out_if_stmt = malloc(sizeof(ast_if_stmt_t));
  SOLC_AST_INIT_HEADER(out_if_stmt, pos, SOLC_AST_TYPE_STMT_IF);
  out_if_stmt->condition_expr_ast = condition_expr_ast;
  out_if_stmt->attrib_list_ast = attrib_list_ast;
  out_if_stmt->stmt_ast = stmt_ast;
  out_if_stmt->else_ast = else_ast;
  return SOLC_AST(out_if_stmt);
}

void solc_ast_stmt_if_destroy(solc_ast_t *if_ast)
{
  SOLC_ASSUME(if_ast != nullptr && if_ast->type == SOLC_AST_TYPE_STMT_IF);
  SOLC_AST_CAST(if_data, if_ast, ast_if_stmt_t);
  solc_ast_destroy_if_exists(if_data->condition_expr_ast);
  solc_ast_destroy_if_exists(if_data->attrib_list_ast);
  solc_ast_destroy_if_exists(if_data->stmt_ast);
  solc_ast_destroy_if_exists(if_data->else_ast);
  free(if_ast);
}

string_t *solc_ast_stmt_if_build_tree(solc_ast_t *if_ast)
{
  SOLC_ASSUME(if_ast != nullptr && if_ast->type == SOLC_AST_TYPE_STMT_IF);
  SOLC_AST_CAST(if_data, if_ast, ast_if_stmt_t);

  string_t header = string_create_from("STMT_IF");
  string_t **children_vs_v = vector_reserve(string_t *, 4);
  solc_ast_add_to_tree_if_exists(children_vs_v, if_data->condition_expr_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, if_data->attrib_list_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, if_data->stmt_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, if_data->else_ast);

  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t *solc_ast_stmt_if_get_condition_expr_ast(solc_ast_t *if_ast)
{
  SOLC_ASSUME(if_ast != nullptr && if_ast->type == SOLC_AST_TYPE_STMT_IF);
  SOLC_AST_CAST(if_data, if_ast, ast_if_stmt_t);
  return if_data->condition_expr_ast;
}

solc_ast_t *solc_ast_stmt_if_get_attribute_list_ast(solc_ast_t *if_ast)
{
  SOLC_ASSUME(if_ast != nullptr && if_ast->type == SOLC_AST_TYPE_STMT_IF);
  SOLC_AST_CAST(if_data, if_ast, ast_if_stmt_t);
  return if_data->attrib_list_ast;
}

solc_ast_t *solc_ast_stmt_if_get_stmt_ast(solc_ast_t *if_ast)
{
  SOLC_ASSUME(if_ast != nullptr && if_ast->type == SOLC_AST_TYPE_STMT_IF);
  SOLC_AST_CAST(if_data, if_ast, ast_if_stmt_t);
  return if_data->stmt_ast;
}

solc_ast_t *solc_ast_stmt_if_get_else_ast(solc_ast_t *if_ast)
{
  SOLC_ASSUME(if_ast != nullptr && if_ast->type == SOLC_AST_TYPE_STMT_IF);
  SOLC_AST_CAST(if_data, if_ast, ast_if_stmt_t);
  return if_data->else_ast;
}
