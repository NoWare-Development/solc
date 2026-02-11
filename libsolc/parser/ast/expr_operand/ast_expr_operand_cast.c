#include "solc/parser/ast.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
  solc_ast_t *expr_ast;
} ast_expr_operand_cast_t;

solc_ast_t *solc_ast_expr_operand_cast_to_create(sz pos, solc_ast_t *type_ast,
                                                 solc_ast_t *expr_ast)
{
  SOLC_ASSUME(type_ast != nullptr && expr_ast != nullptr);
  ast_expr_operand_cast_t *out_expr_operand_cast =
    malloc(sizeof(ast_expr_operand_cast_t));
  SOLC_AST_INIT_HEADER(out_expr_operand_cast, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_CAST_TO);
  out_expr_operand_cast->type_ast = type_ast;
  out_expr_operand_cast->expr_ast = expr_ast;
  return SOLC_AST(out_expr_operand_cast);
}

void solc_ast_expr_operand_cast_to_destroy(solc_ast_t *cast_to_expr_operand_ast)
{
  SOLC_ASSUME(cast_to_expr_operand_ast != nullptr &&
              cast_to_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_CAST_TO);
  SOLC_AST_CAST(cast_to_expr_operand_data, cast_to_expr_operand_ast,
                ast_expr_operand_cast_t);
  SOLC_ASSUME(cast_to_expr_operand_data->type_ast != nullptr &&
              cast_to_expr_operand_data->expr_ast != nullptr);
  solc_ast_destroy(cast_to_expr_operand_data->type_ast);
  solc_ast_destroy(cast_to_expr_operand_data->expr_ast);
  free(cast_to_expr_operand_ast);
}

sz solc_ast_expr_operand_cast_to_to_string(char *buf, sz n,
                                           solc_ast_t *cast_to_expr_operand_ast)
{
  SOLC_ASSUME(buf != nullptr && cast_to_expr_operand_ast != nullptr &&
              cast_to_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_CAST_TO);

  SOLC_TODO("Expression operand cast to to string.");

  return 0;
}
