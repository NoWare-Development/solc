#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  char value;
} ast_expr_operand_symbol_t;

solc_ast_t *solc_ast_expr_operand_symbol_create(sz pos, char value)
{
  ast_expr_operand_symbol_t *out_expr_operand_symbol =
    malloc(sizeof(ast_expr_operand_symbol_t));
  SOLC_AST_INIT_HEADER(out_expr_operand_symbol, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_SYMBOL);
  out_expr_operand_symbol->value = value;
  return SOLC_AST(out_expr_operand_symbol);
}

void solc_ast_expr_operand_symbol_destroy(solc_ast_t *symbol_expr_operand_ast)
{
  SOLC_ASSUME(symbol_expr_operand_ast != nullptr &&
              symbol_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_SYMBOL);
  free(symbol_expr_operand_ast);
}

string_t *
solc_ast_expr_operand_symbol_build_tree(solc_ast_t *symbol_expr_operand_ast)
{
  SOLC_ASSUME(symbol_expr_operand_ast != nullptr &&
              symbol_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_SYMBOL);
  SOLC_AST_CAST(symbol_expr_operand_data, symbol_expr_operand_ast,
                ast_expr_operand_symbol_t);

  string_t header = string_create_from("EXPR_OPERAND_SYMBOL { value: ");

  if (isgraph(symbol_expr_operand_data->value)) {
    string_append_char(&header, '\'');
    string_append_char(&header, symbol_expr_operand_data->value);
    string_append_cstr(&header, "\' ");
  } else {
    const char *special = nullptr;
    switch (symbol_expr_operand_data->value) {
    case '\n':
      special = "'\\n' ";
      break;
    case '\r':
      special = "'\\r' ";
      break;
    case '\t':
      special = "'\\t' ";
      break;

    default:
      break;
    }

    if (special != nullptr) {
      string_append_cstr(&header, special);
    }
  }

  string_append_char(&header, '(');
  char *buf = malloc(sizeof(char) * 3);
  snprintf(buf, 3, "%02X", symbol_expr_operand_data->value);
  string_append_cstr(&header, buf);
  free(buf);
  string_append_cstr(&header, ") }");

  string_t *out_v = vector_reserve(string_t, 1);
  vector_push(out_v, header);
  return out_v;
}
