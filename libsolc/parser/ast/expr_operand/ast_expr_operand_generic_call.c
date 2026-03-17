#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast/ast_group_generic.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **arg_asts_v;
  solc_ast_t *generic_type_list_ast;
  char *callee_name;
} ast_expr_operand_generic_call_t;

solc_ast_t *
solc_ast_expr_operand_generic_call_create(sz pos, const char *callee_name,
                                          solc_ast_t *generic_type_list_ast)
{
  SOLC_ASSUME(callee_name != nullptr);
  const sz callee_name_len = strlen(callee_name) + 1;
  ast_expr_operand_generic_call_t *out_expr_operand_generic_call =
    malloc(sizeof(ast_expr_operand_generic_call_t) + callee_name_len);
  SOLC_AST_INIT_HEADER(out_expr_operand_generic_call, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_GENERIC_CALL);
  out_expr_operand_generic_call->arg_asts_v = vector_create(solc_ast_t *);
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
  SOLC_ASSUME(generic_call_expr_operand_data->arg_asts_v != nullptr);
  for (sz i = 0, arg_asts_v_size = vector_get_length(
                   generic_call_expr_operand_data->arg_asts_v);
       i < arg_asts_v_size; i++) {
    solc_ast_destroy_if_exists(generic_call_expr_operand_data->arg_asts_v[i]);
  }
  vector_destroy(generic_call_expr_operand_data->arg_asts_v);
  solc_ast_destroy_if_exists(
    generic_call_expr_operand_data->generic_type_list_ast);
  free(generic_call_expr_operand_data);
}

void solc_ast_expr_operand_generic_call_add_argument(
  solc_ast_t *generic_call_expr_operand_ast, solc_ast_t *argument_ast)
{
  SOLC_ASSUME(generic_call_expr_operand_ast != nullptr &&
              generic_call_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_GENERIC_CALL);
  SOLC_AST_CAST(generic_call_expr_operand_data, generic_call_expr_operand_ast,
                ast_expr_operand_generic_call_t);
  SOLC_ASSUME(generic_call_expr_operand_data->arg_asts_v != nullptr);

  vector_push(generic_call_expr_operand_data->arg_asts_v, argument_ast);
}

string_t *solc_ast_expr_operand_generic_call_build_tree(
  solc_ast_t *generic_call_expr_operand_ast)
{
  SOLC_ASSUME(generic_call_expr_operand_ast != nullptr &&
              generic_call_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_GENERIC_CALL);
  SOLC_AST_CAST(generic_call_expr_operand_data, generic_call_expr_operand_ast,
                ast_expr_operand_generic_call_t);
  SOLC_ASSUME(generic_call_expr_operand_data->arg_asts_v != nullptr &&
              generic_call_expr_operand_data->callee_name != nullptr);

  string_t header =
    string_create_from("EXPR_OPERAND_GENERIC_CALL { callee_name: \"");
  string_append_cstr(&header, generic_call_expr_operand_data->callee_name);
  string_append_cstr(&header, "\" }");

  sz arg_asts_v_size =
    vector_get_length(generic_call_expr_operand_data->arg_asts_v);

  string_t **children_vs_v = vector_reserve(string_t *, 1 + arg_asts_v_size);
  solc_ast_add_to_tree_if_exists(
    children_vs_v, generic_call_expr_operand_data->generic_type_list_ast);

  for (sz i = 0; i < arg_asts_v_size; i++) {
    solc_ast_add_to_tree_if_exists(
      children_vs_v, generic_call_expr_operand_data->arg_asts_v[i]);
  }

  return ast_build_tree(&header, children_vs_v);
}

const char *solc_ast_expr_operand_generic_call_get_callee_name(
  solc_ast_t *generic_call_expr_operand_ast)
{
  SOLC_ASSUME(generic_call_expr_operand_ast != nullptr &&
              generic_call_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_GENERIC_CALL);
  SOLC_AST_CAST(generic_call_expr_operand_data, generic_call_expr_operand_ast,
                ast_expr_operand_generic_call_t);
  SOLC_ASSUME(generic_call_expr_operand_data->callee_name != nullptr);
  return generic_call_expr_operand_data->callee_name;
}

solc_ast_t *solc_ast_expr_operand_generic_call_get_generic_type_list_ast(
  solc_ast_t *generic_call_expr_operand_ast)
{
  SOLC_ASSUME(generic_call_expr_operand_ast != nullptr &&
              generic_call_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_GENERIC_CALL);
  SOLC_AST_CAST(generic_call_expr_operand_data, generic_call_expr_operand_ast,
                ast_expr_operand_generic_call_t);
  return generic_call_expr_operand_data->generic_type_list_ast;
}

solc_ast_t **solc_ast_expr_operand_generic_call_get_argument_asts(
  solc_ast_t *generic_call_expr_operand_ast, sz *out_n)
{
  SOLC_ASSUME(generic_call_expr_operand_ast != nullptr &&
              generic_call_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_GENERIC_CALL);
  SOLC_AST_CAST(generic_call_expr_operand_data, generic_call_expr_operand_ast,
                ast_expr_operand_generic_call_t);
  SOLC_ASSUME(generic_call_expr_operand_data->arg_asts_v != nullptr);
  if SOLC_LIKELY (out_n != nullptr)
    *out_n = vector_get_length(generic_call_expr_operand_data->arg_asts_v);
  return generic_call_expr_operand_data->arg_asts_v;
}
