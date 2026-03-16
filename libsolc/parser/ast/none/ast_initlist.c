#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **init_elements_v;
} ast_initlist_t;

solc_ast_t *solc_ast_initlist_create(sz pos)
{
  ast_initlist_t *out_initlist = malloc(sizeof(ast_initlist_t));
  SOLC_AST_INIT_HEADER(out_initlist, pos, SOLC_AST_TYPE_NONE_INITLIST);
  out_initlist->init_elements_v = vector_create(solc_ast_t *);
  return SOLC_AST(out_initlist);
}

void solc_ast_initlist_destroy(solc_ast_t *initlist_ast)
{
  SOLC_ASSUME(initlist_ast != nullptr &&
              initlist_ast->type == SOLC_AST_TYPE_NONE_INITLIST);
  SOLC_AST_CAST(initlist_data, initlist_ast, ast_initlist_t);
  SOLC_ASSUME(initlist_data->init_elements_v != nullptr);
  for (sz i = 0, init_elements_v_len =
                   vector_get_length(initlist_data->init_elements_v);
       i < init_elements_v_len; i++)
    solc_ast_destroy_if_exists(initlist_data->init_elements_v[i]);
  vector_destroy(initlist_data->init_elements_v);
  free(initlist_ast);
}

void solc_ast_initlist_add_element(solc_ast_t *initlist_ast,
                                   solc_ast_t *initlist_element_ast)
{
  SOLC_ASSUME(initlist_ast != nullptr &&
              initlist_ast->type == SOLC_AST_TYPE_NONE_INITLIST);
  SOLC_AST_CAST(initlist_data, initlist_ast, ast_initlist_t);
  SOLC_ASSUME(initlist_data->init_elements_v != nullptr);
  vector_push(initlist_data->init_elements_v, initlist_element_ast);
}

string_t *solc_ast_initlist_build_tree(solc_ast_t *initlist_ast)
{
  SOLC_ASSUME(initlist_ast != nullptr &&
              initlist_ast->type == SOLC_AST_TYPE_NONE_INITLIST);
  SOLC_AST_CAST(initlist_data, initlist_ast, ast_initlist_t);
  SOLC_ASSUME(initlist_data->init_elements_v != nullptr);

  string_t header = string_create_from("INITLIST");
  sz init_elements_v_size = vector_get_length(initlist_data->init_elements_v);
  if (init_elements_v_size == 0) {
    string_t *out_v = vector_reserve(string_t, 1);
    vector_push(out_v, header);
    return out_v;
  }

  string_t **children_vs_v = vector_reserve(string_t *, init_elements_v_size);
  for (sz i = 0; i < init_elements_v_size; i++)
    solc_ast_add_to_tree_if_exists(children_vs_v,
                                   initlist_data->init_elements_v[i]);

  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t **solc_ast_initlist_get_element_asts(solc_ast_t *initlist_ast,
                                                sz *out_n)
{
  SOLC_ASSUME(initlist_ast != nullptr &&
              initlist_ast->type == SOLC_AST_TYPE_NONE_INITLIST);
  SOLC_AST_CAST(initlist_data, initlist_ast, ast_initlist_t);
  SOLC_ASSUME(initlist_data->init_elements_v != nullptr);
  if SOLC_LIKELY (out_n != nullptr)
    *out_n = vector_get_length(initlist_data->init_elements_v);
  return initlist_data->init_elements_v;
}
