#ifndef __SOLC_AST_GROUP_INITLIST_H__
#define __SOLC_AST_GROUP_INITLIST_H__

#include <solc/parser/ast.h>

solc_ast_t *solc_ast_initlist_entry_create(sz pos, solc_ast_t *expr_ast);
void solc_ast_initlist_entry_destroy(solc_ast_t *initlist_entry_ast);
sz solc_ast_initlist_entry_to_string(char *buf, sz n,
                                     solc_ast_t *initlist_entry_ast);

solc_ast_t *solc_ast_initlist_entry_explicit_create(sz pos, const char *name,
                                                    solc_ast_t *expr_ast);
void solc_ast_initlist_entry_explicit_destroy(
  solc_ast_t *initlist_entry_explicit_ast);
sz solc_ast_initlist_entry_explicit_to_string(
  char *buf, sz n, solc_ast_t *initlist_entry_explicit_ast);

solc_ast_t *solc_ast_initlist_entry_explicit_array_element_create(
  sz pos, solc_ast_t *index_expr_ast, solc_ast_t *expr_ast);
void solc_ast_initlist_entry_explicit_array_element_destroy(
  solc_ast_t *initlist_entry_explicit_array_element_ast);
sz solc_ast_initlist_entry_explicit_array_element_to_string(
  char *buf, sz n, solc_ast_t *initlist_entry_explicit_array_element_ast);

#endif // __SOLC_AST_GROUP_INITLIST_H__
