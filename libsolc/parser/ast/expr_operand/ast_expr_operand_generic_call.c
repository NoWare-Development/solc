#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *call_args_ast;
  solc_ast_t *generic_type_list_ast;
  char *callee_name;
} ast_expr_operand_generic_call_t;

solc_ast_t *
solc_ast_expr_operand_generic_call_create(sz pos, const char *callee_name,
                                          solc_ast_t *call_args_ast,
                                          solc_ast_t *generic_type_list_ast)
{
  SOLC_ASSUME(callee_name != nullptr && call_args_ast != nullptr &&
              generic_type_list_ast != nullptr);
  const sz callee_name_len = strlen(callee_name) + 1;
  ast_expr_operand_generic_call_t *out_expr_operand_generic_call =
    malloc(sizeof(ast_expr_operand_generic_call_t) + callee_name_len);
  SOLC_AST_INIT_HEADER(out_expr_operand_generic_call, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_GENERIC_CALL);
  out_expr_operand_generic_call->call_args_ast = call_args_ast;
  out_expr_operand_generic_call->generic_type_list_ast = generic_type_list_ast;
  out_expr_operand_generic_call->callee_name =
    (char *)out_expr_operand_generic_call +
    sizeof(ast_expr_operand_generic_call_t);
  memcpy(out_expr_operand_generic_call->callee_name, callee_name,
         callee_name_len);
  return SOLC_AST(out_expr_operand_generic_call);
}

void solc_ast_expr_operand_generic_call_destroy(
  solc_ast_t *generic_call_expr_operand_ast)
{
  SOLC_ASSUME(generic_call_expr_operand_ast != nullptr &&
              generic_call_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_GENERIC_CALL);
  SOLC_AST_CAST(generic_call_expr_operand_data, generic_call_expr_operand_ast,
                ast_expr_operand_generic_call_t);
  SOLC_ASSUME(generic_call_expr_operand_data->call_args_ast != nullptr &&
              generic_call_expr_operand_data->generic_type_list_ast != nullptr);
  solc_ast_destroy(generic_call_expr_operand_data->call_args_ast);
  solc_ast_destroy(generic_call_expr_operand_data->generic_type_list_ast);
  free(generic_call_expr_operand_data);
}

sz solc_ast_expr_operand_generic_call_to_string(
  char *buf, sz n, solc_ast_t *generic_call_expr_operand_ast)
{
  SOLC_ASSUME(buf != nullptr && generic_call_expr_operand_ast != nullptr &&
              generic_call_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_GENERIC_CALL);

  SOLC_TODO("Generic call expression operand to string.");

  return 0;
}
