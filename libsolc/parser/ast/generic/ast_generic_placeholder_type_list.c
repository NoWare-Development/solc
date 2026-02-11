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

sz solc_ast_generic_placeholder_type_list_to_string(
  char *buf, sz n, solc_ast_t *generic_placeholder_type_list_ast)
{
  SOLC_ASSUME(buf != nullptr && generic_placeholder_type_list_ast != nullptr &&
              generic_placeholder_type_list_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE_LIST);

  SOLC_TODO("Generic placeholder type list to string.");

  return 0;
}
