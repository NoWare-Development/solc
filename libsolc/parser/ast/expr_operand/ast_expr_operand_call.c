#include "containers/string.h"
#include "containers/vector.h"
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
  SOLC_ASSUME(callee_name != nullptr && call_args_ast != nullptr &&
              call_args_ast->type == SOLC_AST_TYPE_NONE_FUNC_ARGLIST);
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
  solc_ast_func_arglist_destroy(call_expr_operand_data->call_args_ast);
  free(call_expr_operand_ast);
}

string_t *
solc_ast_expr_operand_call_build_tree(solc_ast_t *call_expr_operand_ast)
{
  SOLC_ASSUME(call_expr_operand_ast != nullptr &&
              call_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_CALL);
  SOLC_AST_CAST(call_expr_operand_data, call_expr_operand_ast,
                ast_expr_operand_call_t);
  SOLC_ASSUME(call_expr_operand_data->call_args_ast != nullptr &&
              call_expr_operand_data->call_args_ast->type ==
                SOLC_AST_TYPE_NONE_FUNC_ARGLIST &&
              call_expr_operand_data->callee_name != nullptr);

  string_t header = string_create_from("EXPR_OPERAND_CALL { callee_name: \"");
  string_append_cstr(&header, call_expr_operand_data->callee_name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 1);
  vector_push(children_vs_v, solc_ast_func_arglist_build_tree(
                               call_expr_operand_data->call_args_ast));

  return ast_build_tree(&header, children_vs_v);
}
