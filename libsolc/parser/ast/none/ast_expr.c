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

sz solc_ast_expr_to_string(char *buf, sz n, solc_ast_t *expr_ast)
{
  SOLC_ASSUME(buf != nullptr && expr_ast != nullptr &&
              expr_ast->type == SOLC_AST_TYPE_NONE_EXPR);

  SOLC_TODO("Expression to string.");
  return 0;
}
