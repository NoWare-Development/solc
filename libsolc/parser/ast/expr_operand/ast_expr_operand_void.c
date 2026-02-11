#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

solc_ast_t *solc_ast_expr_operand_void_create(sz pos)
{
  solc_ast_t *out_void_expr_operand = malloc(sizeof(solc_ast_t));
  out_void_expr_operand->token_pos = pos;
  out_void_expr_operand->type = SOLC_AST_TYPE_EXPR_OPERAND_VOID;
  return out_void_expr_operand;
}

void solc_ast_expr_operand_void_destroy(solc_ast_t *void_expr_operand_ast)
{
  SOLC_ASSUME(void_expr_operand_ast != nullptr &&
              void_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_VOID);
  free(void_expr_operand_ast);
}

sz solc_ast_expr_operand_void_to_string(char *buf, sz n,
                                        solc_ast_t *void_expr_operand_ast)
{
  SOLC_ASSUME(buf != nullptr && void_expr_operand_ast != nullptr &&
              void_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_VOID);

  return snprintf(buf, n, "EXPR_OPERAND_VOID");
}
