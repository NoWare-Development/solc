#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **type_asts_v;
} ast_generic_type_list_t;

solc_ast_t *solc_ast_generic_type_list_create(sz pos)
{
  ast_generic_type_list_t *out_generic_type_list =
    malloc(sizeof(ast_generic_type_list_t));
  SOLC_AST_INIT_HEADER(out_generic_type_list, pos,
                       SOLC_AST_TYPE_GENERIC_TYPE_LIST);
  out_generic_type_list->type_asts_v = vector_create(solc_ast_t *);
  return SOLC_AST(out_generic_type_list);
}

void solc_ast_generic_type_list_destroy(solc_ast_t *generic_type_list_ast)
{
  SOLC_ASSUME(generic_type_list_ast != nullptr &&
              generic_type_list_ast->type == SOLC_AST_TYPE_GENERIC_TYPE_LIST);
  SOLC_AST_CAST(generic_type_list_data, generic_type_list_ast,
                ast_generic_type_list_t);
  SOLC_ASSUME(generic_type_list_data->type_asts_v != nullptr);
  for (sz i = 0, type_asts_v_size =
                   vector_get_length(generic_type_list_data->type_asts_v);
       i < type_asts_v_size; i++)
    solc_ast_destroy_if_exists(generic_type_list_data->type_asts_v[i]);
  vector_destroy(generic_type_list_data->type_asts_v);
  free(generic_type_list_data);
}

void solc_ast_generic_type_list_add_type(solc_ast_t *generic_type_list_ast,
                                         solc_ast_t *type_ast)
{
  SOLC_ASSUME(generic_type_list_ast != nullptr &&
              generic_type_list_ast->type == SOLC_AST_TYPE_GENERIC_TYPE_LIST);
  SOLC_AST_CAST(generic_type_list_data, generic_type_list_ast,
                ast_generic_type_list_t);
  SOLC_ASSUME(generic_type_list_data->type_asts_v != nullptr);
  vector_push(generic_type_list_data->type_asts_v, type_ast);
}

string_t *
solc_ast_generic_type_list_build_tree(solc_ast_t *generic_type_list_ast)
{
  SOLC_ASSUME(generic_type_list_ast != nullptr &&
              generic_type_list_ast->type == SOLC_AST_TYPE_GENERIC_TYPE_LIST);
  SOLC_AST_CAST(generic_type_list_data, generic_type_list_ast,
                ast_generic_type_list_t);
  SOLC_ASSUME(generic_type_list_data->type_asts_v != nullptr);
  string_t header = string_create_from("GENERIC_TYPE_LIST");
  sz type_asts_v_size = vector_get_length(generic_type_list_data->type_asts_v);
  if (type_asts_v_size == 0) {
    // NOTE: Like with generic_placeholder_type_list I'm not sure that this
    // is a valid case, but keep it for now.
    string_t *out_v = vector_reserve(string_t, 1);
    vector_push(out_v, header);
    return out_v;
  }

  string_t **children_vs_v = vector_reserve(string_t *, type_asts_v_size);
  for (sz i = 0; i < type_asts_v_size; i++)
    solc_ast_add_to_tree_if_exists(children_vs_v,
                                   generic_type_list_data->type_asts_v[i]);

  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t **
solc_ast_generic_type_list_get_type_asts(solc_ast_t *generic_type_list_ast,
                                         sz *out_n)
{
  SOLC_ASSUME(generic_type_list_ast != nullptr &&
              generic_type_list_ast->type == SOLC_AST_TYPE_GENERIC_TYPE_LIST);
  SOLC_AST_CAST(generic_type_list_data, generic_type_list_ast,
                ast_generic_type_list_t);
  SOLC_ASSUME(generic_type_list_data->type_asts_v != nullptr);
  if SOLC_LIKELY (out_n != nullptr)
    *out_n = vector_get_length(generic_type_list_data->type_asts_v);
  return generic_type_list_data->type_asts_v;
}
