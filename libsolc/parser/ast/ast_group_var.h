#ifndef __SOLC_AST_GROUP_VAR_H__
#define __SOLC_AST_GROUP_VAR_H__

#include "containers/string.h"
#include <solc/parser/ast.h>

solc_ast_t *solc_ast_var_decl_create(sz pos, const char *name,
                                     solc_ast_t *type_ast,
                                     solc_ast_t *attribute_list_ast);
void solc_ast_var_decl_destroy(solc_ast_t *var_decl_ast);
string_t *solc_ast_var_decl_build_tree(solc_ast_t *var_decl_ast);
const char *solc_ast_var_decl_get_name(solc_ast_t *var_decl_ast);
solc_ast_t *solc_ast_var_decl_get_type_ast(solc_ast_t *var_decl_ast);
solc_ast_t *solc_ast_var_decl_get_attribute_list_ast(solc_ast_t *var_decl_ast);

solc_ast_t *solc_ast_var_def_create(sz pos, const char *name,
                                    solc_ast_t *type_ast, solc_ast_t *expr_ast,
                                    solc_ast_t *attribute_list_ast);
void solc_ast_var_def_destroy(solc_ast_t *var_def_ast);
string_t *solc_ast_var_def_build_tree(solc_ast_t *var_def_ast);
const char *solc_ast_var_def_get_name(solc_ast_t *var_def_ast);
solc_ast_t *solc_ast_var_def_get_type_ast(solc_ast_t *var_def_ast);
solc_ast_t *solc_ast_var_def_get_expr_ast(solc_ast_t *var_def_ast);
solc_ast_t *solc_ast_var_def_get_attribute_list_ast(solc_ast_t *var_def_ast);

#endif // __SOLC_AST_GROUP_VAR_H__
