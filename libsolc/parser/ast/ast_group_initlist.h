#ifndef __SOLC_AST_GROUP_INITLIST_H__
#define __SOLC_AST_GROUP_INITLIST_H__

#include "containers/string.h"
#include <solc/parser/ast.h>

solc_ast_t *solc_ast_initlist_entry_create(sz pos, solc_ast_t *expr_ast);
void solc_ast_initlist_entry_destroy(solc_ast_t *initlist_entry_ast);
string_t *solc_ast_initlist_entry_build_tree(solc_ast_t *initlist_entry_ast);

solc_ast_t *solc_ast_initlist_entry_explicit_create(sz pos, const char *name,
                                                    solc_ast_t *expr_ast);
void solc_ast_initlist_entry_explicit_destroy(
  solc_ast_t *initlist_entry_explicit_ast);
string_t *solc_ast_initlist_entry_explicit_build_tree(
  solc_ast_t *initlist_entry_explicit_ast);

solc_ast_t *solc_ast_initlist_entry_explicit_array_element_create(
  sz pos, const char *name, solc_ast_t *index_expr_ast, solc_ast_t *expr_ast);
void solc_ast_initlist_entry_explicit_array_element_destroy(
  solc_ast_t *initlist_entry_explicit_array_element_ast);
string_t *solc_ast_initlist_entry_explicit_array_element_build_tree(
  solc_ast_t *initlist_entry_explicit_array_element_ast);

#endif // __SOLC_AST_GROUP_INITLIST_H__
