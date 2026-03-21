#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **attrib_asts_v;
} ast_attribute_list_t;

solc_ast_t *solc_ast_attribute_list_create(sz pos)
{
  ast_attribute_list_t *out_attrib_list = malloc(sizeof(ast_attribute_list_t));
  SOLC_AST_INIT_HEADER(out_attrib_list, pos, SOLC_AST_TYPE_NONE_ATTRIBUTE_LIST);
  out_attrib_list->attrib_asts_v = vector_create(solc_ast_t *);
  return SOLC_AST(out_attrib_list);
}

void solc_ast_attribute_list_destroy(solc_ast_t *attribute_list_ast)
{
  SOLC_ASSUME(attribute_list_ast != nullptr &&
              attribute_list_ast->type == SOLC_AST_TYPE_NONE_ATTRIBUTE_LIST);
  SOLC_AST_CAST(attribute_list_data, attribute_list_ast, ast_attribute_list_t);
  SOLC_ASSUME(attribute_list_data->attrib_asts_v != nullptr);
  for (sz i = 0, n = vector_get_length(attribute_list_data->attrib_asts_v);
       i < n; i++)
    solc_ast_destroy_if_exists(attribute_list_data->attrib_asts_v[i]);
  vector_destroy(attribute_list_data->attrib_asts_v);
  free(attribute_list_data);
}

void solc_ast_attribute_list_add_attribute(solc_ast_t *attribute_list_ast,
                                           solc_ast_t *attribute_ast)
{
  SOLC_ASSUME(attribute_list_ast != nullptr &&
              attribute_list_ast->type == SOLC_AST_TYPE_NONE_ATTRIBUTE_LIST);
  SOLC_AST_CAST(attribute_list_data, attribute_list_ast, ast_attribute_list_t);
  SOLC_ASSUME(attribute_list_data->attrib_asts_v != nullptr);
  vector_push(attribute_list_data->attrib_asts_v, attribute_ast);
}

string_t *solc_ast_attribute_list_build_tree(solc_ast_t *attribute_list_ast)
{
  SOLC_ASSUME(attribute_list_ast != nullptr &&
              attribute_list_ast->type == SOLC_AST_TYPE_NONE_ATTRIBUTE_LIST);
  SOLC_AST_CAST(attribute_list_data, attribute_list_ast, ast_attribute_list_t);
  SOLC_ASSUME(attribute_list_data->attrib_asts_v != nullptr);
  string_t header = string_create_from("ATTRIBUTE_LIST");
  const sz attribs_n = vector_get_length(attribute_list_data->attrib_asts_v);
  if SOLC_UNLIKELY (attribs_n == 0) {
    string_t *out_v = vector_reserve(string_t, 1);
    vector_push(out_v, header);
    return out_v;
  }

  string_t **children_vs_v = vector_reserve(string_t *, attribs_n);
  for (sz i = 0; i < attribs_n; i++)
    solc_ast_add_to_tree_if_exists(children_vs_v,
                                   attribute_list_data->attrib_asts_v[i]);

  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t **
solc_ast_attribute_list_get_attributes(solc_ast_t *attribute_list_ast,
                                       sz *out_n)
{
  SOLC_ASSUME(attribute_list_ast != nullptr &&
              attribute_list_ast->type == SOLC_AST_TYPE_NONE_ATTRIBUTE_LIST);
  SOLC_AST_CAST(attribute_list_data, attribute_list_ast, ast_attribute_list_t);
  SOLC_ASSUME(attribute_list_data->attrib_asts_v != nullptr);
  if SOLC_LIKELY (out_n != nullptr)
    *out_n = vector_get_length(attribute_list_data->attrib_asts_v);
  return attribute_list_data->attrib_asts_v;
}
