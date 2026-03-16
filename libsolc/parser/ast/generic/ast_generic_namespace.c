#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast/ast_group_generic.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *generic_type_list_ast;
  solc_ast_t *subobject_ast;
  char *name;
} ast_generic_namespace_t;

solc_ast_t *solc_ast_generic_namespace_create(sz pos, const char *name,
                                              solc_ast_t *generic_type_list_ast,
                                              solc_ast_t *subobject_ast)
{
  SOLC_ASSUME(name != nullptr);
  const sz name_len = strlen(name) + 1;
  ast_generic_namespace_t *out_generic_namespace =
    malloc(sizeof(ast_generic_namespace_t) + name_len);
  SOLC_AST_INIT_HEADER(out_generic_namespace, pos,
                       SOLC_AST_TYPE_GENERIC_NAMESPACE);
  out_generic_namespace->generic_type_list_ast = generic_type_list_ast;
  out_generic_namespace->subobject_ast = subobject_ast;
  out_generic_namespace->name =
    (char *)out_generic_namespace + sizeof(ast_generic_namespace_t);
  memcpy(out_generic_namespace->name, name, name_len);
  return SOLC_AST(out_generic_namespace);
}

void solc_ast_generic_namespace_destroy(solc_ast_t *generic_namespace_ast)
{
  SOLC_ASSUME(generic_namespace_ast != nullptr &&
              generic_namespace_ast->type == SOLC_AST_TYPE_GENERIC_NAMESPACE);
  SOLC_AST_CAST(generic_namespace_data, generic_namespace_ast,
                ast_generic_namespace_t);
  solc_ast_destroy_if_exists(generic_namespace_data->generic_type_list_ast);
  solc_ast_destroy_if_exists(generic_namespace_data->subobject_ast);
  free(generic_namespace_ast);
}

void solc_ast_generic_namespace_set_subobject(solc_ast_t *generic_namespace_ast,
                                              solc_ast_t *subobject_ast)
{
  if SOLC_UNLIKELY (generic_namespace_ast == nullptr)
    return;

  SOLC_ASSUME(generic_namespace_ast->type == SOLC_AST_TYPE_GENERIC_NAMESPACE);
  SOLC_AST_CAST(generic_namespace_data, generic_namespace_ast,
                ast_generic_namespace_t);

  solc_ast_destroy_if_exists(generic_namespace_data->subobject_ast);
  generic_namespace_data->subobject_ast = subobject_ast;
}

string_t *
solc_ast_generic_namespace_build_tree(solc_ast_t *generic_namespace_ast)
{
  SOLC_ASSUME(generic_namespace_ast != nullptr &&
              generic_namespace_ast->type == SOLC_AST_TYPE_GENERIC_NAMESPACE);
  SOLC_AST_CAST(generic_namespace_data, generic_namespace_ast,
                ast_generic_namespace_t);
  SOLC_ASSUME(generic_namespace_data->name != nullptr);

  string_t header = string_create_from("GENERIC_NAMESPACE { name: \"");
  string_append_cstr(&header, generic_namespace_data->name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 2);
  solc_ast_add_to_tree_if_exists(children_vs_v,
                                 generic_namespace_data->generic_type_list_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v,
                                 generic_namespace_data->subobject_ast);

  return ast_build_tree(&header, children_vs_v);
}

const char *
solc_ast_generic_namespace_get_name(solc_ast_t *generic_namespace_ast)
{
  SOLC_ASSUME(generic_namespace_ast != nullptr &&
              generic_namespace_ast->type == SOLC_AST_TYPE_GENERIC_NAMESPACE);
  SOLC_AST_CAST(generic_namespace_data, generic_namespace_ast,
                ast_generic_namespace_t);
  SOLC_ASSUME(generic_namespace_data->name != nullptr);
  return generic_namespace_data->name;
}

solc_ast_t *solc_ast_generic_namespace_get_generic_type_list_ast(
  solc_ast_t *generic_namespace_ast)
{
  SOLC_ASSUME(generic_namespace_ast != nullptr &&
              generic_namespace_ast->type == SOLC_AST_TYPE_GENERIC_NAMESPACE);
  SOLC_AST_CAST(generic_namespace_data, generic_namespace_ast,
                ast_generic_namespace_t);
  return generic_namespace_data->generic_type_list_ast;
}

solc_ast_t *
solc_ast_generic_namespace_get_subobject_ast(solc_ast_t *generic_namespace_ast)
{
  SOLC_ASSUME(generic_namespace_ast != nullptr &&
              generic_namespace_ast->type == SOLC_AST_TYPE_GENERIC_NAMESPACE);
  SOLC_AST_CAST(generic_namespace_data, generic_namespace_ast,
                ast_generic_namespace_t);
  return generic_namespace_data->subobject_ast;
}
