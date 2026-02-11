#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *call_args_ast;
  char *callee_name;
} ast_expr_operand_call_t;

solc_ast_t *solc_ast_expr_operand_call_create(sz pos, const char *callee_name,
                                              solc_ast_t *call_args_ast)
{
  SOLC_ASSUME(callee_name != nullptr && call_args_ast != nullptr);
  const sz callee_name_len = strlen(callee_name) + 1;
  ast_expr_operand_call_t *out_call_expr_operand =
    malloc(sizeof(ast_expr_operand_call_t) + callee_name_len);
  SOLC_AST_INIT_HEADER(out_call_expr_operand, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_CALL);
  out_call_expr_operand->call_args_ast = call_args_ast;
  out_call_expr_operand->callee_name =
    (char *)out_call_expr_operand + sizeof(ast_expr_operand_call_t);
  memcpy(out_call_expr_operand->callee_name, callee_name, callee_name_len);
  return SOLC_AST(out_call_expr_operand);
}

void solc_ast_expr_operand_call_destroy(solc_ast_t *call_expr_operand_ast)
{
  SOLC_ASSUME(call_expr_operand_ast != nullptr &&
              call_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_CALL);
  SOLC_AST_CAST(call_expr_operand_data, call_expr_operand_ast,
                ast_expr_operand_call_t);
  SOLC_ASSUME(call_expr_operand_data->call_args_ast != nullptr);
  solc_ast_destroy(call_expr_operand_data->call_args_ast);
  free(call_expr_operand_ast);
}

sz solc_ast_expr_operand_call_to_string(char *buf, sz n,
                                        solc_ast_t *call_expr_operand_ast)
{
  SOLC_ASSUME(buf != nullptr && call_expr_operand_ast != nullptr &&
              call_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_CALL);

  SOLC_TODO("Call expression operand to string.");

  return 0;
}
