#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
  char *name;
} ast_enum_element_t;

solc_ast_t *solc_ast_enum_element_create(sz pos, const char *name,
                                         solc_ast_t *expr_ast)
{
  SOLC_ASSUME(name != nullptr);
  const sz name_len = strlen(name) + 1;
  ast_enum_element_t *out_enum_element =
    malloc(sizeof(ast_enum_element_t) + name_len);
  SOLC_AST_INIT_HEADER(out_enum_element, pos, SOLC_AST_TYPE_NONE_ENUM_ELEMENT);
  out_enum_element->expr_ast = expr_ast;
  out_enum_element->name =
    (char *)out_enum_element + sizeof(ast_enum_element_t);
  memcpy(out_enum_element->name, name, name_len);
  return SOLC_AST(out_enum_element);
}

void solc_ast_enum_element_destroy(solc_ast_t *enum_element_ast)
{
  SOLC_ASSUME(enum_element_ast != nullptr &&
              enum_element_ast->type == SOLC_AST_TYPE_NONE_ENUM_ELEMENT);
  SOLC_AST_CAST(enum_element_data, enum_element_ast, ast_enum_element_t);
  solc_ast_destroy_if_exists(enum_element_data->expr_ast);
  free(enum_element_data);
}

string_t *solc_ast_enum_element_build_tree(solc_ast_t *enum_element_ast)
{
  SOLC_ASSUME(enum_element_ast != nullptr &&
              enum_element_ast->type == SOLC_AST_TYPE_NONE_ENUM_ELEMENT);
  SOLC_AST_CAST(enum_element_data, enum_element_ast, ast_enum_element_t);
  SOLC_ASSUME(enum_element_data->name != nullptr);
  string_t *out_v;
  sz n = strlen(enum_element_data->name) + 128;
  char *buf = malloc(sizeof(char) * n);
  snprintf(buf, n, "ENUM_ELEMENT { name: \"%s\" }", enum_element_data->name);
  string_t header = string_create_from(buf);
  free(buf);
  if (enum_element_data->expr_ast == nullptr) {
    out_v = vector_reserve(string_t, 1);
    vector_push(out_v, header);
    return out_v;
  }

  string_t **children_vs_v = vector_reserve(string_t *, 1);
  vector_push(children_vs_v,
              ast_get_build_tree_func(enum_element_data->expr_ast->type)(
                enum_element_data->expr_ast));
  out_v = ast_build_tree(&header, children_vs_v);
  return out_v;
}

const char *solc_ast_enum_element_get_name(solc_ast_t *enum_element_ast)
{
  SOLC_ASSUME(enum_element_ast != nullptr &&
              enum_element_ast->type == SOLC_AST_TYPE_NONE_ENUM_ELEMENT);
  SOLC_AST_CAST(enum_element_data, enum_element_ast, ast_enum_element_t);
  SOLC_ASSUME(enum_element_data->name != nullptr);
  return enum_element_data->name;
}

solc_ast_t *solc_ast_enum_element_get_expr_ast(solc_ast_t *enum_element_ast)
{
  SOLC_ASSUME(enum_element_ast != nullptr &&
              enum_element_ast->type == SOLC_AST_TYPE_NONE_ENUM_ELEMENT);
  SOLC_AST_CAST(enum_element_data, enum_element_ast, ast_enum_element_t);
  return enum_element_data->expr_ast;
}
