#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  long long value;
  char *typespec;
} ast_num_expr_operand_t;

solc_ast_t *solc_ast_expr_operand_num_create(sz pos, long long value,
                                             const char *typespec)
{
  const sz typespec_len = typespec != nullptr ? strlen(typespec) + 1 : 0;
  ast_num_expr_operand_t *out_num_expr_operand =
    malloc(sizeof(ast_num_expr_operand_t) + typespec_len);
  SOLC_AST_INIT_HEADER(out_num_expr_operand, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_NUM);
  out_num_expr_operand->value = value;
  if (typespec == nullptr) {
    out_num_expr_operand->typespec = nullptr;
  } else {
    out_num_expr_operand->typespec =
      (char *)out_num_expr_operand + sizeof(ast_num_expr_operand_t);
    memcpy(out_num_expr_operand->typespec, typespec, typespec_len);
  }
  return SOLC_AST(out_num_expr_operand);
}

void solc_ast_expr_operand_num_destroy(solc_ast_t *num_expr_operand_ast)
{
  SOLC_ASSUME(num_expr_operand_ast != nullptr &&
              num_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_NUM);
  free(num_expr_operand_ast);
}

sz solc_ast_expr_operand_num_to_string(char *buf, sz n,
                                       solc_ast_t *num_expr_operand_ast)
{
  SOLC_ASSUME(buf != nullptr && num_expr_operand_ast != nullptr &&
              num_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_NUM);
  SOLC_AST_CAST(num_expr_operand_data, num_expr_operand_ast,
                ast_num_expr_operand_t);
  return num_expr_operand_data->typespec != nullptr ?
           snprintf(buf, n,
                    "EXPR_OPERAND_NUM { value: %lld, typespec: \"%s\" }",
                    num_expr_operand_data->value,
                    num_expr_operand_data->typespec) :
           snprintf(buf, n, "EXPR_OPERAND_NUM { value: %lld, typespec: - }",
                    num_expr_operand_data->value);
}
