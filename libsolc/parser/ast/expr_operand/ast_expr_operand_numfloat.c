#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  f64 value;
  char *typespec;
} ast_numfloat_expr_operand_t;

solc_ast_t *solc_ast_expr_operand_numfloat_create(sz pos, f64 value,
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
  free(numfloat_expr_operand_ast);
}

string_t *
solc_ast_expr_operand_numfloat_build_tree(solc_ast_t *numfloat_expr_operand_ast)
{
  SOLC_ASSUME(numfloat_expr_operand_ast != nullptr &&
              numfloat_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_NUMFLOAT);
  SOLC_AST_CAST(numfloat_expr_operand_data, numfloat_expr_operand_ast,
                ast_numfloat_expr_operand_t);
  const sz typespec_len = numfloat_expr_operand_data->typespec != nullptr ?
                            strlen(numfloat_expr_operand_data->typespec) :
                            0;
  char *buf = malloc(sizeof(char) * 256 + typespec_len);
  if (numfloat_expr_operand_data->typespec != nullptr) {
    snprintf(buf, 256 + typespec_len,
             "EXPR_OPERAND_NUM { value: %lf, typespec: \"%s\" }",
             numfloat_expr_operand_data->value,
             numfloat_expr_operand_data->typespec);
  } else {
    snprintf(buf, 256, "EXPR_OPERAND_NUM { value: %lf, typespec: <NONE> }",
             numfloat_expr_operand_data->value);
  }
  string_t *out_v = vector_reserve(string_t, 1);
  vector_push(out_v, string_create_from(buf));
  free(buf);
  return out_v;
}
