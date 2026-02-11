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
       i < type_asts_v_size; i++) {
    SOLC_ASSUME(generic_type_list_data->type_asts_v[i] != nullptr);
    solc_ast_destroy(generic_type_list_data->type_asts_v[i]);
  }
  vector_destroy(generic_type_list_data->type_asts_v);
  free(generic_type_list_data);
}

void solc_ast_generic_type_list_add_type(solc_ast_t *generic_type_list_ast,
                                         solc_ast_t *type_ast)
{
  SOLC_ASSUME(generic_type_list_ast != nullptr &&
              generic_type_list_ast->type == SOLC_AST_TYPE_GENERIC_TYPE_LIST &&
              type_ast != nullptr);
  SOLC_AST_CAST(generic_type_list_data, generic_type_list_ast,
                ast_generic_type_list_t);
  SOLC_ASSUME(generic_type_list_data->type_asts_v != nullptr);
  vector_push(generic_type_list_data->type_asts_v, type_ast);
}

sz solc_ast_generic_type_list_to_string(char *buf, sz n,
                                        solc_ast_t *generic_type_list_ast)
{
  SOLC_ASSUME(buf != nullptr && generic_type_list_ast != nullptr &&
              generic_type_list_ast->type == SOLC_AST_TYPE_GENERIC_TYPE_LIST);

  SOLC_TODO("Generic type list to string.");

  return 0;
}
