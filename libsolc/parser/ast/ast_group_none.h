#ifndef __SOLC_AST_GROUP_NONE_H__
#define __SOLC_AST_GROUP_NONE_H__

#include "containers/string.h"
#include <solc/parser/ast.h>
#include "parser/ast_op_types.h"

solc_ast_t *solc_ast_err_create(sz pos, const char *reason);
void solc_ast_err_destroy(solc_ast_t *err_ast);
string_t *solc_ast_err_build_tree(solc_ast_t *err_ast);

solc_ast_t *solc_ast_root_create(void);
void solc_ast_root_destroy(solc_ast_t *root_ast);
void solc_ast_root_add_top_statement(solc_ast_t *root_ast,
                                     solc_ast_t *top_stmt_ast);
string_t *solc_ast_root_build_tree(solc_ast_t *root_ast);

solc_ast_t *solc_ast_expr_create(sz pos, solc_ast_t *lhs_ast,
                                 solc_ast_t *rhs_ast,
                                 expr_operator_type_t op_type);
void solc_ast_expr_destroy(solc_ast_t *expr_ast);
string_t *solc_ast_expr_build_tree(solc_ast_t *expr_ast);

solc_ast_t *solc_ast_module_create(sz pos, const char *name,
                                   solc_ast_t *submodule_ast);
void solc_ast_module_destroy(solc_ast_t *module_ast);
string_t *solc_ast_module_build_tree(solc_ast_t *module_ast);

solc_ast_t *solc_ast_import_create(sz pos, solc_ast_t *module_ast);
void solc_ast_import_destroy(solc_ast_t *import_ast);
string_t *solc_ast_import_build_tree(solc_ast_t *import_ast);

solc_ast_t *solc_ast_typedef_create(sz pos, solc_ast_t *type_ast,
                                    const char *name);
void solc_ast_typedef_destroy(solc_ast_t *typedef_ast);
string_t *solc_ast_typedef_build_tree(solc_ast_t *typedef_ast);

solc_ast_t *solc_ast_exported_func_create(sz pos, const char *name,
                                          solc_ast_t *type_ast,
                                          solc_ast_t *arg_list_ast,
                                          solc_ast_t *block_ast);
void solc_ast_exported_func_destroy(solc_ast_t *exported_func_ast);
string_t *solc_ast_exported_func_build_tree(solc_ast_t *exported_func_ast);

solc_ast_t *solc_ast_extern_func_create(sz pos, const char *name,
                                        solc_ast_t *type_ast,
                                        solc_ast_t *arg_list_ast);
void solc_ast_extern_func_destroy(solc_ast_t *extern_func_ast);
string_t *solc_ast_extern_func_build_tree(solc_ast_t *extern_func_ast);

solc_ast_t *solc_ast_qualifier_create(sz pos, const char *name,
                                      solc_ast_t *qualified_ast);
void solc_ast_qualifier_destroy(solc_ast_t *qualifier_ast);
string_t *solc_ast_qualifier_build_tree(solc_ast_t *qualifier_ast);

solc_ast_t *solc_ast_none_create(sz pos);
void solc_ast_none_destroy(solc_ast_t *none_ast);
string_t *solc_ast_none_build_tree(solc_ast_t *none_ast);

solc_ast_t *solc_ast_variadic_create(sz pos);
void solc_ast_variadic_destroy(solc_ast_t *variadic_ast);
string_t *solc_ast_variadic_build_tree(solc_ast_t *variadic_ast);

solc_ast_t *solc_ast_struct_create(sz pos, const char *name);
void solc_ast_struct_destroy(solc_ast_t *struct_ast);
void solc_ast_struct_add_child(solc_ast_t *struct_ast, solc_ast_t *child_ast);
string_t *solc_ast_struct_build_tree(solc_ast_t *struct_ast);

solc_ast_t *solc_ast_union_create(sz pos, const char *name);
void solc_ast_union_destroy(solc_ast_t *union_ast);
void solc_ast_union_add_child(solc_ast_t *union_ast, solc_ast_t *child_ast);
string_t *solc_ast_union_build_tree(solc_ast_t *union_ast);

solc_ast_t *solc_ast_enum_create(sz pos, const char *name);
void solc_ast_enum_destroy(solc_ast_t *enum_ast);
void solc_ast_enum_add_element(solc_ast_t *enum_ast,
                               solc_ast_t *enum_element_ast);
string_t *solc_ast_enum_build_tree(solc_ast_t *enum_ast);

solc_ast_t *solc_ast_enum_element_create(sz pos, const char *name,
                                         solc_ast_t *expr_ast);
void solc_ast_enum_element_destroy(solc_ast_t *enum_element_ast);
string_t *solc_ast_enum_element_build_tree(solc_ast_t *enum_element_ast);
const char *solc_ast_enum_element_get_name(solc_ast_t *enum_element_ast);
solc_ast_t *solc_ast_enum_element_get_expr_ast(solc_ast_t *enum_element_ast);

solc_ast_t *solc_ast_func_create(sz pos, const char *name, solc_ast_t *type_ast,
                                 solc_ast_t *arg_list_ast,
                                 solc_ast_t *block_ast);
void solc_ast_func_destroy(solc_ast_t *func_ast);
string_t *solc_ast_func_build_tree(solc_ast_t *func_ast);
const char *solc_ast_func_get_name(solc_ast_t *func_ast);
solc_ast_t *solc_ast_func_get_type_ast(solc_ast_t *func_ast);
solc_ast_t *solc_ast_func_get_arg_list_ast(solc_ast_t *func_ast);
solc_ast_t *solc_ast_func_get_block_ast(solc_ast_t *func_ast);

solc_ast_t *solc_ast_func_arglist_create(sz pos);
void solc_ast_func_arglist_destroy(solc_ast_t *arg_list_ast);
void solc_ast_func_arglist_add_element(solc_ast_t *arg_list_ast,
                                       solc_ast_t *arg_list_element_ast);
string_t *solc_ast_func_arglist_build_tree(solc_ast_t *arg_list_ast);

solc_ast_t *solc_ast_namespace_create(sz pos, const char *name,
                                      solc_ast_t *subobject);
void solc_ast_namespace_destroy(solc_ast_t *namespace_ast);
string_t *solc_ast_namespace_build_tree(solc_ast_t *namespace_ast);

solc_ast_t *solc_ast_initlist_create(sz pos);
void solc_ast_initlist_destroy(solc_ast_t *initlist_ast);
void solc_ast_initlist_add_element(solc_ast_t *initlist_ast,
                                   solc_ast_t *initlist_element_ast);
string_t *solc_ast_initlist_build_tree(solc_ast_t *initlist_ast);

solc_ast_t *solc_ast_prefix_expr_create(sz pos, solc_ast_t *operand,
                                        expr_operator_type_t *ops_v);
void solc_ast_prefix_expr_destroy(solc_ast_t *prefix_expr_ast);
string_t *solc_ast_prefix_expr_build_tree(solc_ast_t *prefix_expr_ast);

solc_ast_t *solc_ast_func_explicit_create(sz pos, const char *name,
                                          solc_ast_t *type_ast,
                                          solc_ast_t *arg_list_ast,
                                          solc_ast_t *block_ast);
void solc_ast_func_explicit_destroy(solc_ast_t *func_explicit_ast);
string_t *solc_ast_func_explicit_build_tree(solc_ast_t *func_explicit_ast);

#endif // __SOLC_AST_GROUP_NONE_H__
