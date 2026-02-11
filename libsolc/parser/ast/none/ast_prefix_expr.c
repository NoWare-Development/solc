#include "containers/vector.h"
#include "parser/ast_op_types.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *operand;
  expr_operator_type_t *operators;
} ast_prefix_expr_t;

solc_ast_t *solc_ast_prefix_expr_create(sz pos, solc_ast_t *operand)
{
  SOLC_ASSUME(operand != nullptr);

  ast_prefix_expr_t *out_prefix_expr = malloc(sizeof(ast_prefix_expr_t));
  SOLC_AST_INIT_HEADER(out_prefix_expr, pos, SOLC_AST_TYPE_NONE_PREFIX_EXPR);
  out_prefix_expr->operand = operand;
  out_prefix_expr->operators = vector_create(expr_operator_type_t);
  return SOLC_AST(out_prefix_expr);
}

void solc_ast_prefix_expr_destroy(solc_ast_t *prefix_expr_ast)
{
  SOLC_ASSUME(prefix_expr_ast != nullptr &&
              prefix_expr_ast->type == SOLC_AST_TYPE_NONE_PREFIX_EXPR);

  SOLC_AST_CAST(prefix_expr_data, prefix_expr_ast, ast_prefix_expr_t);
  SOLC_ASSUME(prefix_expr_data->operand != nullptr &&
              prefix_expr_data->operators != nullptr);
  solc_ast_destroy(prefix_expr_data->operand);
  vector_destroy(prefix_expr_data->operators);
  free(prefix_expr_ast);
}

void solc_ast_prefix_expr_add_operator(solc_ast_t *prefix_expr_ast,
                                       expr_operator_type_t prefix_operator)
{
  SOLC_ASSUME(prefix_expr_ast != nullptr &&
              prefix_expr_ast->type == SOLC_AST_TYPE_NONE_PREFIX_EXPR);

  SOLC_AST_CAST(prefix_expr_data, prefix_expr_ast, ast_prefix_expr_t);
  SOLC_ASSUME(prefix_expr_data->operators != nullptr);
  vector_push(prefix_expr_data->operators, prefix_operator);
}

sz solc_ast_prefix_expr_to_string(char *buf, sz n, solc_ast_t *prefix_expr_ast)
{
  SOLC_ASSUME(buf != nullptr && prefix_expr_ast != nullptr &&
              prefix_expr_ast->type == SOLC_AST_TYPE_NONE_PREFIX_EXPR);

  SOLC_TODO("Prefix expression to string.");

  return 0;
}
