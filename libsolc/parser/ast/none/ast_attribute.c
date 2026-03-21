#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *name;
  solc_ast_t **arg_asts_v;
} ast_attribute_t;

solc_ast_t *solc_ast_attribute_create(sz pos, const char *name)
{
  SOLC_ASSUME(name != nullptr);
  const sz name_len = strlen(name) + 1;
  ast_attribute_t *out_attrib = malloc(sizeof(ast_attribute_t) + name_len);
  SOLC_AST_INIT_HEADER(out_attrib, pos, SOLC_AST_TYPE_NONE_ATTRIBUTE);
  out_attrib->name = (char *)out_attrib + sizeof(ast_attribute_t);
  memcpy(out_attrib->name, name, name_len);
  out_attrib->arg_asts_v = vector_create(solc_ast_t *);
  return SOLC_AST(out_attrib);
}

void solc_ast_attribute_destroy(solc_ast_t *attribute_ast)
{
  SOLC_ASSUME(attribute_ast != nullptr &&
              attribute_ast->type == SOLC_AST_TYPE_NONE_ATTRIBUTE);
  SOLC_AST_CAST(attribute_data, attribute_ast, ast_attribute_t);
  SOLC_ASSUME(attribute_data->arg_asts_v != nullptr);
  const sz args_n = vector_get_length(attribute_data->arg_asts_v);
  for (sz i = 0; i < args_n; i++)
    solc_ast_destroy_if_exists(attribute_data->arg_asts_v[i]);
  vector_destroy(attribute_data->arg_asts_v);
  free(attribute_data);
}

void solc_ast_attribute_add_argument(solc_ast_t *attribute_ast,
                                     solc_ast_t *expr_ast)
{
  SOLC_ASSUME(attribute_ast != nullptr &&
              attribute_ast->type == SOLC_AST_TYPE_NONE_ATTRIBUTE);
  SOLC_AST_CAST(attribute_data, attribute_ast, ast_attribute_t);
  SOLC_ASSUME(attribute_data->arg_asts_v != nullptr);
  vector_push(attribute_data->arg_asts_v, expr_ast);
}

string_t *solc_ast_attribute_build_tree(solc_ast_t *attribute_ast)
{
  SOLC_ASSUME(attribute_ast != nullptr &&
              attribute_ast->type == SOLC_AST_TYPE_NONE_ATTRIBUTE);
  SOLC_AST_CAST(attribute_data, attribute_ast, ast_attribute_t);
  SOLC_ASSUME(attribute_data->arg_asts_v != nullptr &&
              attribute_data->name != nullptr);

  const sz n =
    sizeof("ATTRIBUTE { name: \"\" }") + strlen(attribute_data->name);
  char *buf = malloc(sizeof(char) * (n + 1));
  snprintf(buf, n, "ATTRIBUTE { name: \"%s\" }", attribute_data->name);
  string_t header = string_create_from(buf);
  free(buf);

  const sz args_n = vector_get_length(attribute_data->arg_asts_v);
  if (args_n == 0) {
    string_t *out_v = vector_reserve(string_t, 1);
    vector_push(out_v, header);
    return out_v;
  }

  string_t **children_vs_v = vector_reserve(string_t *, args_n);
  for (sz i = 0; i < args_n; i++)
    solc_ast_add_to_tree_if_exists(children_vs_v,
                                   attribute_data->arg_asts_v[i]);

  return ast_build_tree(&header, children_vs_v);
}

const char *solc_ast_attribute_get_name(solc_ast_t *attribute_ast)
{
  SOLC_ASSUME(attribute_ast != nullptr &&
              attribute_ast->type == SOLC_AST_TYPE_NONE_ATTRIBUTE);
  SOLC_AST_CAST(attribute_data, attribute_ast, ast_attribute_t);
  SOLC_ASSUME(attribute_data->name != nullptr);
  return attribute_data->name;
}

solc_ast_t **solc_ast_attribute_get_arguments(solc_ast_t *attribute_ast,
                                              sz *out_n)
{
  SOLC_ASSUME(attribute_ast != nullptr &&
              attribute_ast->type == SOLC_AST_TYPE_NONE_ATTRIBUTE);
  SOLC_AST_CAST(attribute_data, attribute_ast, ast_attribute_t);
  SOLC_ASSUME(attribute_data->arg_asts_v != nullptr);
  if SOLC_LIKELY (out_n != nullptr)
    *out_n = vector_get_length(attribute_data->arg_asts_v);
  return attribute_data->arg_asts_v;
}
