#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **arg_asts_v;
  char *callee_name;
} ast_expr_operand_call_t;

solc_ast_t *solc_ast_expr_operand_call_create(sz pos, const char *callee_name)
{
  SOLC_ASSUME(callee_name != nullptr);
  const sz callee_name_len = strlen(callee_name) + 1;
  ast_expr_operand_call_t *out_call_expr_operand =
    malloc(sizeof(ast_expr_operand_call_t) + callee_name_len);
  SOLC_AST_INIT_HEADER(out_call_expr_operand, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_CALL);
  out_call_expr_operand->arg_asts_v = vector_create(solc_ast_t *);
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
  SOLC_ASSUME(call_expr_operand_data->arg_asts_v != nullptr);
  for (sz i = 0, arg_asts_v_size =
                   vector_get_length(call_expr_operand_data->arg_asts_v);
       i < arg_asts_v_size; i++) {
    solc_ast_destroy_if_exists(call_expr_operand_data->arg_asts_v[i]);
  }
  vector_destroy(call_expr_operand_data->arg_asts_v);
  free(call_expr_operand_ast);
}

void solc_ast_expr_operand_call_add_argument(solc_ast_t *call_expr_operand_ast,
                                             solc_ast_t *argument_ast)
{
  SOLC_ASSUME(call_expr_operand_ast != nullptr &&
              call_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_CALL);
  SOLC_AST_CAST(call_expr_operand_data, call_expr_operand_ast,
                ast_expr_operand_call_t);
  SOLC_ASSUME(call_expr_operand_data->arg_asts_v != nullptr);

  vector_push(call_expr_operand_data->arg_asts_v, argument_ast);
}

string_t *
solc_ast_expr_operand_call_build_tree(solc_ast_t *call_expr_operand_ast)
{
  SOLC_ASSUME(call_expr_operand_ast != nullptr &&
              call_expr_operand_ast->type == SOLC_AST_TYPE_EXPR_OPERAND_CALL);
  SOLC_AST_CAST(call_expr_operand_data, call_expr_operand_ast,
                ast_expr_operand_call_t);
  SOLC_ASSUME(call_expr_operand_data->arg_asts_v != nullptr &&
              call_expr_operand_data->callee_name != nullptr);

  string_t header = string_create_from("EXPR_OPERAND_CALL { callee_name: \"");
  string_append_cstr(&header, call_expr_operand_data->callee_name);
  string_append_cstr(&header, "\" }");

  sz args_num = vector_get_length(call_expr_operand_data->arg_asts_v);
  if (args_num == 0) {
    string_t *out_v = vector_reserve(string_t, 1);
    vector_push(out_v, header);
    return out_v;
  }

  string_t **children_vs_v = vector_reserve(string_t *, args_num);
  for (sz i = 0; i < args_num; i++) {
    solc_ast_add_to_tree_if_exists(children_vs_v,
                                   call_expr_operand_data->arg_asts_v[i]);
  }

  return ast_build_tree(&header, children_vs_v);
}
