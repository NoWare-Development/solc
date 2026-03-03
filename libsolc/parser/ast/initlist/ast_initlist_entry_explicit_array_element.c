#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *index_expr_ast;
  solc_ast_t *expr_ast;
  char *name;
} ast_initlist_entry_explicit_array_element_t;

solc_ast_t *solc_ast_initlist_entry_explicit_array_element_create(
  sz pos, const char *name, solc_ast_t *index_expr_ast, solc_ast_t *expr_ast)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;

  ast_initlist_entry_explicit_array_element_t
    *out_initlist_entry_explicit_array_element =
      malloc(sizeof(ast_initlist_entry_explicit_array_element_t) + name_len);
  SOLC_AST_INIT_HEADER(out_initlist_entry_explicit_array_element, pos,
                       SOLC_AST_TYPE_INITLIST_ENTRY_EXPLICIT_ARRAY_ELEMENT);
  out_initlist_entry_explicit_array_element->index_expr_ast = index_expr_ast;
  out_initlist_entry_explicit_array_element->expr_ast = expr_ast;
  out_initlist_entry_explicit_array_element->name =
    (char *)out_initlist_entry_explicit_array_element +
    sizeof(ast_initlist_entry_explicit_array_element_t);
  memcpy(out_initlist_entry_explicit_array_element->name, name, name_len);
  return SOLC_AST(out_initlist_entry_explicit_array_element);
}

void solc_ast_initlist_entry_explicit_array_element_destroy(
  solc_ast_t *initlist_entry_explicit_array_element_ast)
{
  SOLC_ASSUME(initlist_entry_explicit_array_element_ast != nullptr &&
              initlist_entry_explicit_array_element_ast->type ==
                SOLC_AST_TYPE_INITLIST_ENTRY_EXPLICIT_ARRAY_ELEMENT);
  SOLC_AST_CAST(initlist_entry_explicit_array_element_data,
                initlist_entry_explicit_array_element_ast,
                ast_initlist_entry_explicit_array_element_t);
  solc_ast_destroy_if_exists(
    initlist_entry_explicit_array_element_data->index_expr_ast);
  solc_ast_destroy_if_exists(
    initlist_entry_explicit_array_element_data->expr_ast);
  free(initlist_entry_explicit_array_element_data);
}

string_t *solc_ast_initlist_entry_explicit_array_element_build_tree(
  solc_ast_t *initlist_entry_explicit_array_element_ast)
{
  SOLC_ASSUME(initlist_entry_explicit_array_element_ast != nullptr &&
              initlist_entry_explicit_array_element_ast->type ==
                SOLC_AST_TYPE_INITLIST_ENTRY_EXPLICIT_ARRAY_ELEMENT);
  SOLC_AST_CAST(initlist_entry_explicit_array_element_data,
                initlist_entry_explicit_array_element_ast,
                ast_initlist_entry_explicit_array_element_t);
  SOLC_ASSUME(initlist_entry_explicit_array_element_data->name != nullptr);

  string_t header =
    string_create_from("INITLIST_ENTRY_EXPLICIT_ARRAY_ELEMENT { name: \"");
  string_append_cstr(&header, initlist_entry_explicit_array_element_data->name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 2);
  solc_ast_add_to_tree_if_exists(
    children_vs_v, initlist_entry_explicit_array_element_data->index_expr_ast);
  solc_ast_add_to_tree_if_exists(
    children_vs_v, initlist_entry_explicit_array_element_data->expr_ast);

  return ast_build_tree(&header, children_vs_v);
}
