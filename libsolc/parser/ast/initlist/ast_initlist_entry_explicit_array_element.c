#include "parser/ast_private.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *index_expr_ast;
  solc_ast_t *expr_ast;
} ast_initlist_entry_explicit_array_element_t;

solc_ast_t *solc_ast_initlist_entry_explicit_array_element_create(
  sz pos, solc_ast_t *index_expr_ast, solc_ast_t *expr_ast)
{
  SOLC_ASSUME(index_expr_ast != nullptr && expr_ast != nullptr);
  ast_initlist_entry_explicit_array_element_t
    *out_initlist_entry_explicit_array_element =
      malloc(sizeof(ast_initlist_entry_explicit_array_element_t));
  SOLC_AST_INIT_HEADER(out_initlist_entry_explicit_array_element, pos,
                       SOLC_AST_TYPE_INITLIST_ENTRY_EXPLICIT_ARRAY_ELEMENT);
  out_initlist_entry_explicit_array_element->index_expr_ast = index_expr_ast;
  out_initlist_entry_explicit_array_element->expr_ast = expr_ast;
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
  SOLC_ASSUME(initlist_entry_explicit_array_element_data->index_expr_ast !=
                nullptr &&
              initlist_entry_explicit_array_element_data->expr_ast != nullptr);
  solc_ast_destroy(initlist_entry_explicit_array_element_data->index_expr_ast);
  solc_ast_destroy(initlist_entry_explicit_array_element_data->expr_ast);
  free(initlist_entry_explicit_array_element_data);
}

sz solc_ast_initlist_entry_explicit_array_element_to_string(
  char *buf, sz n, solc_ast_t *initlist_entry_explicit_array_element_ast)
{
  SOLC_ASSUME(buf != nullptr &&
              initlist_entry_explicit_array_element_ast != nullptr &&
              initlist_entry_explicit_array_element_ast->type ==
                SOLC_AST_TYPE_INITLIST_ENTRY_EXPLICIT_ARRAY_ELEMENT);

  SOLC_TODO("Explicit array element initialization list entry to string.");

  return 0;
}
