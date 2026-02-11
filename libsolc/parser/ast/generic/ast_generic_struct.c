#include "containers/vector.h"
#include "parser/ast/ast_group_generic.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *generic_placeholder_type_list_ast;
  solc_ast_t **children_v;
  char *name;
} ast_generic_struct_t;

solc_ast_t *
solc_ast_generic_struct_create(sz pos, const char *name,
                               solc_ast_t *generic_placeholder_type_list_ast)
{
  SOLC_ASSUME(name != nullptr && generic_placeholder_type_list_ast != nullptr &&
              generic_placeholder_type_list_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE_LIST);
  const sz name_len = strlen(name) + 1;
  ast_generic_struct_t *out_generic_struct =
    malloc(sizeof(ast_generic_struct_t) + name_len);
  SOLC_AST_INIT_HEADER(out_generic_struct, pos, SOLC_AST_TYPE_GENERIC_STRUCT);
  out_generic_struct->generic_placeholder_type_list_ast =
    generic_placeholder_type_list_ast;
  out_generic_struct->children_v = vector_create(solc_ast_t *);
  out_generic_struct->name =
    (char *)out_generic_struct + sizeof(ast_generic_struct_t);
  memcpy(out_generic_struct->name, name, name_len);
  return SOLC_AST(out_generic_struct);
}

void solc_ast_generic_struct_destroy(solc_ast_t *generic_struct_ast)
{
  SOLC_ASSUME(generic_struct_ast != nullptr &&
              generic_struct_ast->type == SOLC_AST_TYPE_GENERIC_STRUCT);
  SOLC_AST_CAST(generic_struct_data, generic_struct_ast, ast_generic_struct_t);
  SOLC_ASSUME(generic_struct_data->generic_placeholder_type_list_ast !=
                nullptr &&
              generic_struct_data->generic_placeholder_type_list_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE_LIST &&
              generic_struct_data->children_v != nullptr);
  solc_ast_generic_placeholder_type_list_destroy(
    generic_struct_data->generic_placeholder_type_list_ast);
  for (sz i = 0,
          children_v_size = vector_get_length(generic_struct_data->children_v);
       i < children_v_size; i++) {
    SOLC_ASSUME(generic_struct_data->children_v[i] != nullptr);
    solc_ast_destroy(generic_struct_data->children_v[i]);
  }
  vector_destroy(generic_struct_data->children_v);
  free(generic_struct_data);
}

void solc_ast_generic_struct_append_child(solc_ast_t *generic_struct_ast,
                                          solc_ast_t *child_ast)
{
  SOLC_ASSUME(generic_struct_ast != nullptr &&
              generic_struct_ast->type == SOLC_AST_TYPE_GENERIC_STRUCT &&
              child_ast != nullptr);
  SOLC_AST_CAST(generic_struct_data, generic_struct_ast, ast_generic_struct_t);
  SOLC_ASSUME(generic_struct_data->children_v != nullptr);
  vector_push(generic_struct_data->children_v, child_ast);
}

sz solc_ast_generic_struct_to_string(char *buf, sz n,
                                     solc_ast_t *generic_struct_ast)
{
  SOLC_ASSUME(buf != nullptr && generic_struct_ast != nullptr &&
              generic_struct_ast->type == SOLC_AST_TYPE_GENERIC_STRUCT);

  SOLC_TODO("Generic struct to string.");

  return 0;
}
