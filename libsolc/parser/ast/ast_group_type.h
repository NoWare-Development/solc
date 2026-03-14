#ifndef __SOLC_AST_GROUP_TYPE_H__
#define __SOLC_AST_GROUP_TYPE_H__

#include "containers/string.h"
#include <solc/parser/ast.h>

solc_ast_t *solc_ast_type_plain_create(sz pos, const char *name);
void solc_ast_type_plain_destroy(solc_ast_t *plain_type_ast);
string_t *solc_ast_type_plain_build_tree(solc_ast_t *plain_type_ast);

solc_ast_t *solc_ast_type_array_create(sz pos, solc_ast_t *size_expr_ast,
                                       solc_ast_t *type_ast);
void solc_ast_type_array_destroy(solc_ast_t *array_type_ast);
string_t *solc_ast_type_array_build_tree(solc_ast_t *array_type_ast);

solc_ast_t *solc_ast_type_pointer_create(sz pos, solc_ast_t *type_ast);
void solc_ast_type_pointer_destroy(solc_ast_t *pointer_type_ast);
string_t *solc_ast_type_pointer_build_tree(solc_ast_t *pointer_type_ast);

solc_ast_t *solc_ast_type_funcptr_create(sz pos, solc_ast_t *type_ast,
                                         solc_ast_t *arg_list_ast);
void solc_ast_type_funcptr_destroy(solc_ast_t *funcptr_type_ast);
string_t *solc_ast_type_funcptr_build_tree(solc_ast_t *funcptr_type_ast);

solc_ast_t *solc_ast_type_typeof_create(sz pos, solc_ast_t *expr_ast);
void solc_ast_type_typeof_destroy(solc_ast_t *typeof_type_ast);
string_t *solc_ast_type_typeof_build_tree(solc_ast_t *typeof_type_ast);

#endif // __SOLC_AST_GROUP_TYPE_H__
