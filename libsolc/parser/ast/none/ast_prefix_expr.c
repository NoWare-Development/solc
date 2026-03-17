#include "containers/vector.h"
#include "parser/ast_op_types.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *operand_ast;
  expr_operator_type_t *operators_v;
} ast_prefix_expr_t;

solc_ast_t *solc_ast_prefix_expr_create(sz pos, solc_ast_t *operand_ast,
                                        expr_operator_type_t *operators_v)
{
  SOLC_ASSUME(operators_v != nullptr);

  ast_prefix_expr_t *out_prefix_expr = malloc(sizeof(ast_prefix_expr_t));
  SOLC_AST_INIT_HEADER(out_prefix_expr, pos, SOLC_AST_TYPE_NONE_PREFIX_EXPR);
  out_prefix_expr->operand_ast = operand_ast;
  out_prefix_expr->operators_v = operators_v;
  return SOLC_AST(out_prefix_expr);
}

void solc_ast_prefix_expr_destroy(solc_ast_t *prefix_expr_ast)
{
  SOLC_ASSUME(prefix_expr_ast != nullptr &&
              prefix_expr_ast->type == SOLC_AST_TYPE_NONE_PREFIX_EXPR);
  SOLC_AST_CAST(prefix_expr_data, prefix_expr_ast, ast_prefix_expr_t);
  SOLC_ASSUME(prefix_expr_data->operators_v != nullptr);
  solc_ast_destroy_if_exists(prefix_expr_data->operand_ast);
  vector_destroy(prefix_expr_data->operators_v);
  free(prefix_expr_ast);
}

string_t *solc_ast_prefix_expr_build_tree(solc_ast_t *prefix_expr_ast)
{
  SOLC_ASSUME(prefix_expr_ast != nullptr &&
              prefix_expr_ast->type == SOLC_AST_TYPE_NONE_PREFIX_EXPR);
  SOLC_AST_CAST(prefix_expr_data, prefix_expr_ast, ast_prefix_expr_t);
  SOLC_ASSUME(prefix_expr_data->operators_v != nullptr);
  string_t header = string_create_from("PREFIX_EXPR { operators: \"");
  for (sz i = 0,
          operators_size = vector_get_length(prefix_expr_data->operators_v);
       i < operators_size; i++) {
    string_append_cstr(&header, ast_expr_operator_type_to_string(
                                  prefix_expr_data->operators_v[i]));
  }
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v, prefix_expr_data->operand_ast);

  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t *solc_ast_prefix_expr_get_operand_ast(solc_ast_t *prefix_expr_ast)
{
  SOLC_ASSUME(prefix_expr_ast != nullptr &&
              prefix_expr_ast->type == SOLC_AST_TYPE_NONE_PREFIX_EXPR);
  SOLC_AST_CAST(prefix_expr_data, prefix_expr_ast, ast_prefix_expr_t);
  return prefix_expr_data->operand_ast;
}

expr_operator_type_t *
solc_ast_prefix_expr_get_operators(solc_ast_t *prefix_expr_ast, sz *out_n)
{
  SOLC_ASSUME(prefix_expr_ast != nullptr &&
              prefix_expr_ast->type == SOLC_AST_TYPE_NONE_PREFIX_EXPR);
  SOLC_AST_CAST(prefix_expr_data, prefix_expr_ast, ast_prefix_expr_t);
  SOLC_ASSUME(prefix_expr_data->operators_v != nullptr);
  if SOLC_LIKELY (out_n != nullptr)
    *out_n = vector_get_length(prefix_expr_data->operators_v);
  return prefix_expr_data->operators_v;
}
