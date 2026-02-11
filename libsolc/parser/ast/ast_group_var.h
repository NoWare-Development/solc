#ifndef __SOLC_AST_GROUP_VAR_H__
#define __SOLC_AST_GROUP_VAR_H__

#include <solc/parser/ast.h>

solc_ast_t *solc_ast_var_decl_create(sz pos, const char *name,
                                     solc_ast_t *type_ast);
void solc_ast_var_decl_destroy(solc_ast_t *var_decl_ast);
sz solc_ast_var_decl_to_string(char *buf, sz n, solc_ast_t *var_decl_ast);

solc_ast_t *solc_ast_var_def_create(sz pos, const char *name,
                                    solc_ast_t *type_ast, solc_ast_t *expr_ast);
void solc_ast_var_def_destroy(solc_ast_t *var_def_ast);
sz solc_ast_var_def_to_string(char *buf, sz n, solc_ast_t *var_def_ast);

#endif // __SOLC_AST_GROUP_VAR_H__
