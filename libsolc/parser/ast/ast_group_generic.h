#ifndef __SOLC_AST_GROUP_GENERIC_H__
#define __SOLC_AST_GROUP_GENERIC_H__

#include "containers/string.h"
#include <solc/parser/ast.h>

solc_ast_t *
solc_ast_generic_struct_create(sz pos, const char *name,
                               solc_ast_t *generic_placeholder_type_list_ast);
void solc_ast_generic_struct_destroy(solc_ast_t *generic_struct_ast);
void solc_ast_generic_struct_append_child(solc_ast_t *generic_struct_ast,
                                          solc_ast_t *child_ast);
string_t *solc_ast_generic_struct_build_tree(solc_ast_t *generic_struct_ast);

solc_ast_t *
solc_ast_generic_func_create(sz pos, const char *name, solc_ast_t *type_ast,
                             solc_ast_t *arg_list_ast, solc_ast_t *block_ast,
                             solc_ast_t *generic_placeholder_type_list_ast);
void solc_ast_generic_func_destroy(solc_ast_t *generic_func_ast);
string_t *solc_ast_generic_func_build_tree(solc_ast_t *generic_func_ast);
const char *solc_ast_generic_func_get_name(solc_ast_t *generic_func_ast);
solc_ast_t *solc_ast_generic_func_get_type_ast(solc_ast_t *generic_func_ast);
solc_ast_t *
solc_ast_generic_func_get_arg_list_ast(solc_ast_t *generic_func_ast);
solc_ast_t *solc_ast_generic_func_get_block_ast(solc_ast_t *generic_func_ast);
solc_ast_t *solc_ast_generic_func_get_generic_placeholder_type_list_ast(
  solc_ast_t *generic_func_ast);

solc_ast_t *solc_ast_generic_placeholder_type_list_create(sz pos);
void solc_ast_generic_placeholder_type_list_destroy(
  solc_ast_t *generic_placeholder_type_list_ast);
void solc_ast_generic_placeholder_type_list_add_placeholder_type(
  solc_ast_t *generic_placeholder_type_list_ast,
  solc_ast_t *generic_placeholder_type_ast);
string_t *solc_ast_generic_placeholder_type_list_build_tree(
  solc_ast_t *generic_placeholder_type_list_ast);

solc_ast_t *solc_ast_generic_placeholder_type_create(sz pos, const char *name);
void solc_ast_generic_placeholder_type_destroy(
  solc_ast_t *generic_placeholder_type_ast);
string_t *solc_ast_generic_placeholder_type_build_tree(
  solc_ast_t *generic_placeholder_type_ast);

solc_ast_t *solc_ast_generic_type_list_create(sz pos);
void solc_ast_generic_type_list_destroy(solc_ast_t *generic_type_list_ast);
void solc_ast_generic_type_list_add_type(solc_ast_t *generic_type_list_ast,
                                         solc_ast_t *type_ast);
string_t *
solc_ast_generic_type_list_build_tree(solc_ast_t *generic_type_list_ast);

solc_ast_t *solc_ast_generic_type_create(sz pos, const char *name,
                                         solc_ast_t *generic_type_list_ast);
void solc_ast_generic_type_destroy(solc_ast_t *generic_type_ast);
string_t *solc_ast_generic_type_build_tree(solc_ast_t *generic_type_ast);

solc_ast_t *solc_ast_generic_namespace_create(sz pos, const char *name,
                                              solc_ast_t *generic_type_list_ast,
                                              solc_ast_t *subobject_ast);
void solc_ast_generic_namespace_destroy(solc_ast_t *generic_namespace_ast);
string_t *
solc_ast_generic_namespace_build_tree(solc_ast_t *generic_namespace_ast);

solc_ast_t *solc_ast_generic_func_explicit_create(sz pos,
                                                  solc_ast_t *generic_func_ast);
void solc_ast_generic_func_explicit_destroy(
  solc_ast_t *generic_func_explicit_ast);
string_t *solc_ast_generic_func_explicit_build_tree(
  solc_ast_t *generic_func_explicit_ast);

#endif // __SOLC_AST_GROUP_GENERIC_H__
