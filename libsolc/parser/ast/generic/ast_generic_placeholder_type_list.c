#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast/ast_group_generic.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **placeholder_types_v;
} ast_generic_placeholder_type_list_t;

solc_ast_t *solc_ast_generic_placeholder_type_list_create(sz pos)
{
  ast_generic_placeholder_type_list_t *out_generic_placeholder_type_list =
    malloc(sizeof(ast_generic_placeholder_type_list_t));
  SOLC_AST_INIT_HEADER(out_generic_placeholder_type_list, pos,
                       SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE_LIST);
  out_generic_placeholder_type_list->placeholder_types_v =
    vector_create(solc_ast_t *);
  return SOLC_AST(out_generic_placeholder_type_list);
}

void solc_ast_generic_placeholder_type_list_destroy(
  solc_ast_t *generic_placeholder_type_list_ast)
{
  SOLC_ASSUME(generic_placeholder_type_list_ast != nullptr &&
              generic_placeholder_type_list_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE_LIST);
  SOLC_AST_CAST(generic_placeholder_type_list_data,
                generic_placeholder_type_list_ast,
                ast_generic_placeholder_type_list_t);
  SOLC_ASSUME(generic_placeholder_type_list_data->placeholder_types_v !=
              nullptr);
  for (sz i = 0, placeholder_types_v_size = vector_get_length(
                   generic_placeholder_type_list_data->placeholder_types_v);
       i < placeholder_types_v_size; i++) {
    SOLC_ASSUME(
      generic_placeholder_type_list_data->placeholder_types_v[i] != nullptr &&
      generic_placeholder_type_list_data->placeholder_types_v[i]->type ==
        SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE);
    solc_ast_generic_placeholder_type_destroy(
      generic_placeholder_type_list_data->placeholder_types_v[i]);
  }
  vector_destroy(generic_placeholder_type_list_data->placeholder_types_v);
  free(generic_placeholder_type_list_data);
}

void solc_ast_generic_placeholder_type_list_add_placeholder_type(
  solc_ast_t *generic_placeholder_type_list_ast,
  solc_ast_t *generic_placeholder_type_ast)
{
  SOLC_ASSUME(generic_placeholder_type_list_ast != nullptr &&
              generic_placeholder_type_list_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE_LIST &&
              generic_placeholder_type_ast != nullptr &&
              generic_placeholder_type_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE);
  SOLC_AST_CAST(generic_placeholder_type_list_data,
                generic_placeholder_type_list_ast,
                ast_generic_placeholder_type_list_t);
  SOLC_ASSUME(generic_placeholder_type_list_data->placeholder_types_v !=
              nullptr);
  vector_push(generic_placeholder_type_list_data->placeholder_types_v,
              generic_placeholder_type_ast);
}

string_t *solc_ast_generic_placeholder_type_list_build_tree(
  solc_ast_t *generic_placeholder_type_list_ast)
{
  SOLC_ASSUME(generic_placeholder_type_list_ast != nullptr &&
              generic_placeholder_type_list_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE_LIST);
  SOLC_AST_CAST(generic_placeholder_type_list_data,
                generic_placeholder_type_list_ast,
                ast_generic_placeholder_type_list_t);
  SOLC_ASSUME(generic_placeholder_type_list_data->placeholder_types_v !=
              nullptr);

  string_t header = string_create_from("GENERIC_PLACEHOLDER_TYPE_LIST");
  sz placeholder_types_v_size =
    vector_get_length(generic_placeholder_type_list_data->placeholder_types_v);
  if SOLC_UNLIKELY (placeholder_types_v_size == 0) {
    // NOTE: I don't think that this case is valid
    // but will keep it here for now.
    string_t *out_v = vector_reserve(string_t, 1);
    vector_push(out_v, header);
    return out_v;
  }

  string_t **children_vs_v =
    vector_reserve(string_t *, placeholder_types_v_size);
  for (sz i = 0; i < placeholder_types_v_size; i++) {
    SOLC_ASSUME(
      generic_placeholder_type_list_data->placeholder_types_v[i] != nullptr &&
      generic_placeholder_type_list_data->placeholder_types_v[i]->type ==
        SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE);
    vector_push(children_vs_v,
                solc_ast_generic_placeholder_type_build_tree(
                  generic_placeholder_type_list_data->placeholder_types_v[i]));
  }

  return ast_build_tree(&header, children_vs_v);
}
