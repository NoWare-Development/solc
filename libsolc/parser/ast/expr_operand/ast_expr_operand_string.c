#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *value;
} ast_expr_operand_string_t;

solc_ast_t *solc_ast_expr_operand_string_create(sz pos, const char *value)
{
  SOLC_ASSUME(value != nullptr);
  const sz value_len = strlen(value) + 1;
  ast_expr_operand_string_t *out_expr_operand_string =
    malloc(sizeof(ast_expr_operand_string_t) + value_len);
  SOLC_AST_INIT_HEADER(out_expr_operand_string, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_STRING);
  out_expr_operand_string->value =
    (char *)out_expr_operand_string + sizeof(ast_expr_operand_string_t);
  memcpy(out_expr_operand_string->value, value, value_len);
  return SOLC_AST(out_expr_operand_string);
}

void solc_ast_expr_operand_string_destroy(solc_ast_t *string_expr_operand_ast)
{
  SOLC_ASSUME(string_expr_operand_ast != nullptr &&
              string_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_STRING);
  free(string_expr_operand_ast);
}

string_t *
solc_ast_expr_operand_string_build_tree(solc_ast_t *string_expr_operand_ast)
{
  SOLC_ASSUME(string_expr_operand_ast != nullptr &&
              string_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_STRING);
  SOLC_AST_CAST(string_expr_operand_data, string_expr_operand_ast,
                ast_expr_operand_string_t);
  SOLC_ASSUME(string_expr_operand_data->value != nullptr);

  string_t header = string_create_from("EXPR_OPERAND_STRING { value: \"");
  char *buf =
    malloc(sizeof(char) * (strlen(string_expr_operand_data->value) * 2) + 1);
  char *pbuf = buf;
  for (const char *s = string_expr_operand_data->value; *s; s++) {
    switch (*s) {
    case '\n':
      *pbuf++ = '\\';
      *pbuf++ = 'n';
      break;
    case '\r':
      *pbuf++ = '\\';
      *pbuf++ = 'r';
      break;
    case '\t':
      *pbuf++ = '\\';
      *pbuf++ = 't';
      break;
    default:
      *pbuf++ = *s;
      break;
    }
  }
  *pbuf = 0;
  string_append_cstr(&header, buf);
  free(buf);
  string_append_cstr(&header, "\" }");
  string_t *out_v = vector_reserve(string_t, 1);
  vector_push(out_v, header);
  return out_v;
}

const char *
solc_ast_expr_operand_string_get_value(solc_ast_t *string_expr_operand_ast)
{
  SOLC_ASSUME(string_expr_operand_ast != nullptr &&
              string_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_STRING);
  SOLC_AST_CAST(string_expr_operand_data, string_expr_operand_ast,
                ast_expr_operand_string_t);
  SOLC_ASSUME(string_expr_operand_data->value != nullptr);
  return string_expr_operand_data->value;
}
