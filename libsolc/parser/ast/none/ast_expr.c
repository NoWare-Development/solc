#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_op_types.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *lhs_ast;
  solc_ast_t *rhs_ast;
  expr_operator_type_t operator_type;
} ast_expr_t;

solc_ast_t *solc_ast_expr_create(sz pos, solc_ast_t *lhs_ast,
                                 solc_ast_t *rhs_ast,
                                 expr_operator_type_t op_type)
{
  SOLC_ASSUME(lhs_ast != nullptr && rhs_ast != nullptr);

  ast_expr_t *out_expr = malloc(sizeof(ast_expr_t));
  SOLC_AST_INIT_HEADER(out_expr, pos, SOLC_AST_TYPE_NONE_EXPR);
  out_expr->lhs_ast = lhs_ast;
  out_expr->rhs_ast = rhs_ast;
  out_expr->operator_type = op_type;
  return SOLC_AST(out_expr);
}

void solc_ast_expr_destroy(solc_ast_t *expr_ast)
{
  SOLC_ASSUME(expr_ast != nullptr && expr_ast->type == SOLC_AST_TYPE_NONE_EXPR);

  SOLC_AST_CAST(expr_data, expr_ast, ast_expr_t);
  SOLC_ASSUME(expr_data->lhs_ast != nullptr && expr_data->rhs_ast != nullptr);
  solc_ast_destroy(expr_data->lhs_ast);
  solc_ast_destroy(expr_data->rhs_ast);

  free(expr_ast);
}

string_t *solc_ast_expr_build_tree(solc_ast_t *expr_ast)
{
  SOLC_ASSUME(expr_ast != nullptr && expr_ast->type == SOLC_AST_TYPE_NONE_EXPR);
  SOLC_AST_CAST(expr_data, expr_ast, ast_expr_t);
  SOLC_ASSUME(expr_data->lhs_ast != nullptr && expr_data->rhs_ast != nullptr);
  string_t **children_vs_v = vector_reserve(string_t *, 2);
  vector_push(children_vs_v, ast_get_build_tree_func(expr_data->lhs_ast->type)(
                               expr_data->lhs_ast));
  vector_push(children_vs_v, ast_get_build_tree_func(expr_data->rhs_ast->type)(
                               expr_data->rhs_ast));
  string_t header = string_create_from("EXPR { operator: \"");
  string_append_cstr(
    &header, ast_expr_operator_type_to_string(expr_data->operator_type));
  string_append_cstr(&header, "\" }");
  return ast_build_tree(&header, children_vs_v);
}
