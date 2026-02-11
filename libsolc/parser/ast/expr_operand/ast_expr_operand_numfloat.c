#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  double value;
  char *typespec;
} ast_numfloat_expr_operand_t;

solc_ast_t *solc_ast_expr_operand_numfloat_create(sz pos, double value,
                                                  const char *typespec)
{
  const sz typespec_len = typespec != nullptr ? strlen(typespec) + 1 : 0;
  ast_numfloat_expr_operand_t *out_numfloat_expr_operand =
    malloc(sizeof(ast_numfloat_expr_operand_t) + typespec_len);
  SOLC_AST_INIT_HEADER(out_numfloat_expr_operand, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_NUMFLOAT);
  out_numfloat_expr_operand->value = value;
  if (typespec != nullptr) {
    out_numfloat_expr_operand->typespec =
      (char *)out_numfloat_expr_operand + sizeof(ast_numfloat_expr_operand_t);
    memcpy(out_numfloat_expr_operand->typespec, typespec, typespec_len);
  } else {
    out_numfloat_expr_operand->typespec = nullptr;
  }
  return SOLC_AST(out_numfloat_expr_operand);
}

void solc_ast_expr_operand_numfloat_destroy(
  solc_ast_t *numfloat_expr_operand_ast)
{
  SOLC_ASSUME(numfloat_expr_operand_ast != nullptr &&
              numfloat_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_NUMFLOAT);
}

sz solc_ast_expr_operand_numfloat_to_string(
  char *buf, sz n, solc_ast_t *numfloat_expr_operand_ast)
{
}
