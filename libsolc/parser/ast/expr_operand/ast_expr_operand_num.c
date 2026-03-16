#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef struct {
  SOLC_AST_HEADER;
  u64 value;
  char *typespec;
} ast_num_expr_operand_t;

solc_ast_t *solc_ast_expr_operand_num_create(sz pos, u64 value,
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

string_t *solc_ast_expr_operand_num_build_tree(solc_ast_t *num_expr_operand_ast)
{
  SOLC_ASSUME(num_expr_operand_ast != nullptr &&
              num_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_NUM);
  SOLC_AST_CAST(num_expr_operand_data, num_expr_operand_ast,
                ast_num_expr_operand_t);
  const sz typespec_len = num_expr_operand_data->typespec != nullptr ?
                            strlen(num_expr_operand_data->typespec) :
                            0;
  char *buf = malloc(sizeof(char) * 256 + typespec_len);
  if (num_expr_operand_data->typespec != nullptr) {
    snprintf(buf, 256 + typespec_len,
             "EXPR_OPERAND_NUM { value: %" PRIu64 ", typespec: \"%s\" }",
             num_expr_operand_data->value, num_expr_operand_data->typespec);
  } else {
    snprintf(buf, 256,
             "EXPR_OPERAND_NUM { value: %" PRIu64 ", typespec: <NONE> }",
             num_expr_operand_data->value);
  }
  string_t *out_v = vector_reserve(string_t, 1);
  vector_push(out_v, string_create_from(buf));
  free(buf);
  return out_v;
}

u64 solc_ast_expr_operand_num_get_value(solc_ast_t *num_expr_operand_ast)
{
  SOLC_ASSUME(num_expr_operand_ast != nullptr &&
              num_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_NUM);
  SOLC_AST_CAST(num_expr_operand_data, num_expr_operand_ast,
                ast_num_expr_operand_t);
  return num_expr_operand_data->value;
}

const char *
solc_ast_expr_operand_num_get_typespec(solc_ast_t *num_expr_operand_ast)
{
  SOLC_ASSUME(num_expr_operand_ast != nullptr &&
              num_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_NUM);
  SOLC_AST_CAST(num_expr_operand_data, num_expr_operand_ast,
                ast_num_expr_operand_t);
  return num_expr_operand_data->typespec;
}
