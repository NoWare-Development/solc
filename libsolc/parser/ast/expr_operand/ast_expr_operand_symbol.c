#include "parser/ast_private.h"
#include "solc/parser/ast.h"
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

sz solc_ast_expr_operand_symbol_to_string(char *buf, sz n,
                                          solc_ast_t *symbol_expr_operand_ast)
{
  SOLC_ASSUME(buf != nullptr && symbol_expr_operand_ast != nullptr &&
              symbol_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_SYMBOL);
  SOLC_AST_CAST(symbol_expr_operand_data, symbol_expr_operand_ast,
                ast_expr_operand_symbol_t);
  // TODO: Handle escape characters properly.
  return snprintf(buf, n, "EXPR_OPERAND_SYMBOL { value: '%c' (%d) }",
                  symbol_expr_operand_data->value,
                  symbol_expr_operand_data->value);
}
