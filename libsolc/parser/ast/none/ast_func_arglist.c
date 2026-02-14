#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **elements_v;
} ast_func_arglist_t;

solc_ast_t *solc_ast_func_arglist_create(sz pos)
{
  ast_func_arglist_t *out_func_arglist = malloc(sizeof(ast_func_arglist_t));
  SOLC_AST_INIT_HEADER(out_func_arglist, pos, SOLC_AST_TYPE_NONE_FUNC_ARGLIST);

  out_func_arglist->elements_v = vector_create(solc_ast_t *);

  return SOLC_AST(out_func_arglist);
}

void solc_ast_func_arglist_destroy(solc_ast_t *arg_list_ast)
{
  SOLC_ASSUME(arg_list_ast != nullptr &&
              arg_list_ast->type == SOLC_AST_TYPE_NONE_FUNC_ARGLIST);

  SOLC_AST_CAST(arg_list_data, arg_list_ast, ast_func_arglist_t);
  SOLC_ASSUME(arg_list_data->elements_v != nullptr);
  for (sz i = 0, elements_v_size = vector_get_length(arg_list_data->elements_v);
       i < elements_v_size; i++) {
    solc_ast_destroy(arg_list_data->elements_v[i]);
  }

  vector_destroy(arg_list_data->elements_v);

  free(arg_list_ast);
}

void solc_ast_func_arglist_add_element(solc_ast_t *arg_list_ast,
                                       solc_ast_t *arg_list_element_ast)
{
  SOLC_ASSUME(arg_list_ast != nullptr &&
              arg_list_ast->type == SOLC_AST_TYPE_NONE_FUNC_ARGLIST &&
              arg_list_element_ast != nullptr);

  SOLC_AST_CAST(arg_list_data, arg_list_ast, ast_func_arglist_t);
  SOLC_ASSUME(arg_list_data->elements_v != nullptr);
  vector_push(arg_list_data->elements_v, arg_list_element_ast);
}

string_t *solc_ast_func_arglist_build_tree(solc_ast_t *arg_list_ast)
{
  SOLC_ASSUME(arg_list_ast != nullptr &&
              arg_list_ast->type == SOLC_AST_TYPE_NONE_FUNC_ARGLIST);
  SOLC_AST_CAST(arg_list_data, arg_list_ast, ast_func_arglist_t);
  SOLC_ASSUME(arg_list_data->elements_v != nullptr);

  sz elements_v_size = vector_get_length(arg_list_data->elements_v);
  if (elements_v_size == 0) {
    string_t *out_v = vector_reserve(string_t, 1);
    vector_push(out_v, string_create_from("FUNC_ARGLIST"));
    return out_v;
  }

  string_t header = string_create_from("FUNC_ARGLIST");
  string_t **children_vs_v = vector_reserve(string_t *, elements_v_size);
  for (sz i = 0; i < elements_v_size; i++) {
    SOLC_ASSUME(arg_list_data->elements_v[i] != nullptr);
    vector_push(children_vs_v,
                ast_get_build_tree_func(arg_list_data->elements_v[i]->type)(
                  arg_list_data->elements_v[i]));
  }

  return ast_build_tree(&header, children_vs_v);
}
