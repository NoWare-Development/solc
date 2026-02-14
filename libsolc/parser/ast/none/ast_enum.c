#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **elements_v;
  char *name;
} ast_enum_t;

solc_ast_t *solc_ast_enum_create(sz pos, const char *name)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_enum_t *out_enum_ast = malloc(sizeof(ast_enum_t) + name_len);
  SOLC_AST_INIT_HEADER(out_enum_ast, pos, SOLC_AST_TYPE_NONE_ENUM);
  out_enum_ast->elements_v = vector_create(solc_ast_t *);
  out_enum_ast->name = (char *)out_enum_ast + sizeof(ast_enum_t);
  memcpy(out_enum_ast->name, name, name_len);

  return SOLC_AST(out_enum_ast);
}

void solc_ast_enum_destroy(solc_ast_t *enum_ast)
{
  SOLC_ASSUME(enum_ast != nullptr && enum_ast->type == SOLC_AST_TYPE_NONE_ENUM);

  SOLC_AST_CAST(enum_data, enum_ast, ast_enum_t);
  SOLC_ASSUME(enum_data->elements_v != nullptr);
  for (sz i = 0, elements_v_size = vector_get_length(enum_data->elements_v);
       i < elements_v_size; i++) {
    solc_ast_destroy(enum_data->elements_v[i]);
  }
  vector_destroy(enum_data->elements_v);
  free(enum_data);
}

void solc_ast_enum_add_element(solc_ast_t *enum_ast,
                               solc_ast_t *enum_element_ast)
{
  SOLC_ASSUME(enum_ast != nullptr &&
              enum_ast->type == SOLC_AST_TYPE_NONE_ENUM &&
              enum_element_ast != nullptr &&
              enum_element_ast->type == SOLC_AST_TYPE_NONE_ENUM_ELEMENT);

  SOLC_AST_CAST(enum_data, enum_ast, ast_enum_t);
  SOLC_ASSUME(enum_data->elements_v != nullptr);

  vector_push(enum_data->elements_v, enum_element_ast);
}

string_t *solc_ast_enum_build_tree(solc_ast_t *enum_ast)
{
  SOLC_ASSUME(enum_ast != nullptr && enum_ast->type == SOLC_AST_TYPE_NONE_ENUM);
  SOLC_AST_CAST(enum_data, enum_ast, ast_enum_t);
  SOLC_ASSUME(enum_data->elements_v != nullptr && enum_data->name != nullptr);

  string_t header = string_create_from("ENUM { name: \"");
  string_append_cstr(&header, enum_data->name);
  string_append_cstr(&header, "\" }");

  sz elements_v_size = vector_get_length(enum_data->elements_v);
  string_t **children_vs_v = vector_reserve(string_t *, elements_v_size);
  for (sz i = 0; i < elements_v_size; i++) {
    SOLC_ASSUME(enum_data->elements_v[i] != nullptr &&
                enum_data->elements_v[i]->type ==
                  SOLC_AST_TYPE_NONE_ENUM_ELEMENT);
    vector_push(children_vs_v,
                solc_ast_enum_element_build_tree(enum_data->elements_v[i]));
  }

  return ast_build_tree(&header, children_vs_v);
}
