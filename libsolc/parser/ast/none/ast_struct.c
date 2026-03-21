#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *attribute_list_ast;
  solc_ast_t **children_v;
  char *name;
} ast_struct_t;

solc_ast_t *solc_ast_struct_create(sz pos, const char *name,
                                   solc_ast_t *attribute_list_ast)
{
  SOLC_ASSUME(name != nullptr);
  const sz name_len = strlen(name) + 1;
  ast_struct_t *out_struct = malloc(sizeof(ast_struct_t) + name_len);
  SOLC_AST_INIT_HEADER(out_struct, pos, SOLC_AST_TYPE_NONE_STRUCT);
  out_struct->attribute_list_ast = attribute_list_ast;
  out_struct->children_v = vector_create(solc_ast_t *);
  out_struct->name = (char *)out_struct + sizeof(ast_struct_t);
  memcpy(out_struct->name, name, name_len);
  return SOLC_AST(out_struct);
}

void solc_ast_struct_destroy(solc_ast_t *struct_ast)
{
  SOLC_ASSUME(struct_ast != nullptr &&
              struct_ast->type == SOLC_AST_TYPE_NONE_STRUCT);
  SOLC_AST_CAST(struct_data, struct_ast, ast_struct_t);
  SOLC_ASSUME(struct_data->children_v != nullptr);
  for (sz i = 0, children_v_len = vector_get_length(struct_data->children_v);
       i < children_v_len; i++)
    solc_ast_destroy_if_exists(struct_data->children_v[i]);
  vector_destroy(struct_data->children_v);
  solc_ast_destroy_if_exists(struct_data->attribute_list_ast);
  free(struct_ast);
}

void solc_ast_struct_add_child(solc_ast_t *struct_ast, solc_ast_t *child_ast)
{
  SOLC_ASSUME(struct_ast != nullptr &&
              struct_ast->type == SOLC_AST_TYPE_NONE_STRUCT);
  SOLC_AST_CAST(struct_data, struct_ast, ast_struct_t);
  SOLC_ASSUME(struct_data->children_v != nullptr);
  vector_push(struct_data->children_v, child_ast);
}

string_t *solc_ast_struct_build_tree(solc_ast_t *struct_ast)
{
  SOLC_ASSUME(struct_ast != nullptr &&
              struct_ast->type == SOLC_AST_TYPE_NONE_STRUCT);
  SOLC_AST_CAST(struct_data, struct_ast, ast_struct_t);
  SOLC_ASSUME(struct_data->children_v != nullptr &&
              struct_data->name != nullptr);

  string_t header = string_create_from("STRUCT { name: \"");
  string_append_cstr(&header, struct_data->name);
  string_append_cstr(&header, "\" }");

  sz children_v_size = vector_get_length(struct_data->children_v);
  string_t **children_vs_v = vector_reserve(string_t *, children_v_size + 1);

  solc_ast_add_to_tree_if_exists(children_vs_v,
                                 struct_data->attribute_list_ast);

  for (sz i = 0; i < children_v_size; i++)
    solc_ast_add_to_tree_if_exists(children_vs_v, struct_data->children_v[i]);

  return ast_build_tree(&header, children_vs_v);
}

const char *solc_ast_struct_get_name(solc_ast_t *struct_ast)
{
  SOLC_ASSUME(struct_ast != nullptr &&
              struct_ast->type == SOLC_AST_TYPE_NONE_STRUCT);
  SOLC_AST_CAST(struct_data, struct_ast, ast_struct_t);
  SOLC_ASSUME(struct_data->name != nullptr);
  return struct_data->name;
}

solc_ast_t **solc_ast_struct_get_child_asts(solc_ast_t *struct_ast, sz *out_n)
{
  SOLC_ASSUME(struct_ast != nullptr &&
              struct_ast->type == SOLC_AST_TYPE_NONE_STRUCT);
  SOLC_AST_CAST(struct_data, struct_ast, ast_struct_t);
  SOLC_ASSUME(struct_data->children_v != nullptr);
  if SOLC_LIKELY (out_n != nullptr)
    *out_n = vector_get_length(struct_data->children_v);
  return struct_data->children_v;
}

solc_ast_t *solc_ast_struct_get_attribute_list_ast(solc_ast_t *struct_ast)
{
  SOLC_ASSUME(struct_ast != nullptr &&
              struct_ast->type == SOLC_AST_TYPE_NONE_STRUCT);
  SOLC_AST_CAST(struct_data, struct_ast, ast_struct_t);
  return struct_data->attribute_list_ast;
}
