#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
} ast_expr_operand_sizeof_t;

solc_ast_t *solc_ast_expr_operand_sizeof_create(sz pos, solc_ast_t *type_ast)
{
  SOLC_ASSUME(type_ast != nullptr);
  ast_expr_operand_sizeof_t *out_expr_operand_sizeof =
    malloc(sizeof(ast_expr_operand_sizeof_t));
  SOLC_AST_INIT_HEADER(out_expr_operand_sizeof, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_SIZEOF);
  out_expr_operand_sizeof->type_ast = type_ast;
  return SOLC_AST(out_expr_operand_sizeof);
}

void solc_ast_expr_operand_sizeof_destroy(solc_ast_t *sizeof_expr_operand_ast)
{
  SOLC_ASSUME(sizeof_expr_operand_ast != nullptr &&
              sizeof_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_SIZEOF);
  SOLC_AST_CAST(sizeof_expr_operand_data, sizeof_expr_operand_ast,
                ast_expr_operand_sizeof_t);
  SOLC_ASSUME(sizeof_expr_operand_data->type_ast != nullptr);
  solc_ast_destroy(sizeof_expr_operand_data->type_ast);
  free(sizeof_expr_operand_data);
}

sz solc_ast_expr_operand_sizeof_to_string(char *buf, sz n,
                                          solc_ast_t *sizeof_expr_operand_ast)
{
  SOLC_ASSUME(buf != nullptr && sizeof_expr_operand_ast != nullptr &&
              sizeof_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_SIZEOF);

  SOLC_TODO("Sizeof expression operand to string.");

  return 0;
}
