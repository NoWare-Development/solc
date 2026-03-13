#ifndef __SOLC_AST_GROUP_STMT_H__
#define __SOLC_AST_GROUP_STMT_H__

#include "containers/string.h"
#include <solc/parser/ast.h>

solc_ast_t *solc_ast_stmt_block_create(sz pos);
void solc_ast_stmt_block_destroy(solc_ast_t *block_ast);
void solc_ast_stmt_block_add_stmt(solc_ast_t *block_ast, solc_ast_t *stmt_ast);
string_t *solc_ast_stmt_block_build_tree(solc_ast_t *block_ast);

solc_ast_t *solc_ast_stmt_return_create(sz pos, solc_ast_t *expr_ast);
void solc_ast_stmt_return_destroy(solc_ast_t *return_ast);
string_t *solc_ast_stmt_return_build_tree(solc_ast_t *return_ast);

solc_ast_t *solc_ast_stmt_goto_create(sz pos, const char *label_name);
void solc_ast_stmt_goto_destroy(solc_ast_t *goto_ast);
string_t *solc_ast_stmt_goto_build_tree(solc_ast_t *goto_ast);

solc_ast_t *solc_ast_stmt_label_create(sz pos, const char *name);
void solc_ast_stmt_label_destroy(solc_ast_t *label_ast);
string_t *solc_ast_stmt_label_build_tree(solc_ast_t *label_ast);

solc_ast_t *solc_ast_stmt_break_create(sz pos);
void solc_ast_stmt_break_destroy(solc_ast_t *break_ast);
string_t *solc_ast_stmt_break_build_tree(solc_ast_t *break_ast);

solc_ast_t *solc_ast_stmt_continue_create(sz pos);
void solc_ast_stmt_continue_destroy(solc_ast_t *continue_ast);
string_t *solc_ast_stmt_continue_build_tree(solc_ast_t *continue_ast);

solc_ast_t *solc_ast_stmt_fallthrough_create(sz pos);
void solc_ast_stmt_fallthrough_destroy(solc_ast_t *fallthrough_ast);
string_t *solc_ast_stmt_fallthrough_build_tree(solc_ast_t *fallthrough_ast);

solc_ast_t *solc_ast_stmt_while_create(sz pos, solc_ast_t *condition_expr_ast,
                                       solc_ast_t *stmt_ast);
void solc_ast_stmt_while_destroy(solc_ast_t *while_ast);
string_t *solc_ast_stmt_while_build_tree(solc_ast_t *while_ast);

solc_ast_t *solc_ast_stmt_for_create(sz pos, solc_ast_t *init_stmt_ast,
                                     solc_ast_t *condition_expr_ast,
                                     solc_ast_t *expr_ast,
                                     solc_ast_t *stmt_ast);
void solc_ast_stmt_for_destroy(solc_ast_t *for_ast);
string_t *solc_ast_stmt_for_build_tree(solc_ast_t *for_ast);

solc_ast_t *solc_ast_stmt_dowhile_create(sz pos, solc_ast_t *condition_expr_ast,
                                         solc_ast_t *stmt_ast);
void solc_ast_stmt_dowhile_destroy(solc_ast_t *dowhile_ast);
string_t *solc_ast_stmt_dowhile_build_tree(solc_ast_t *dowhile_ast);

solc_ast_t *solc_ast_stmt_expr_create(sz pos, solc_ast_t *expr_ast);
void solc_ast_stmt_expr_destroy(solc_ast_t *expr_stmt_ast);
string_t *solc_ast_stmt_expr_build_tree(solc_ast_t *expr_stmt_ast);

solc_ast_t *solc_ast_stmt_switch_create(sz pos, solc_ast_t *expr_ast);
void solc_ast_stmt_switch_destroy(solc_ast_t *switch_ast);
void solc_ast_stmt_switch_add_case(solc_ast_t *switch_ast,
                                   solc_ast_t *case_ast);
string_t *solc_ast_stmt_switch_build_tree(solc_ast_t *switch_ast);

solc_ast_t *solc_ast_stmt_case_create(sz pos, solc_ast_t *expr_ast,
                                      solc_ast_t *block_ast);
void solc_ast_stmt_case_destroy(solc_ast_t *case_ast);
string_t *solc_ast_stmt_case_build_tree(solc_ast_t *case_ast);

solc_ast_t *solc_ast_stmt_default_create(sz pos, solc_ast_t *block_ast);
void solc_ast_stmt_default_destroy(solc_ast_t *default_ast);
string_t *solc_ast_stmt_default_build_tree(solc_ast_t *default_ast);

solc_ast_t *solc_ast_stmt_defer_create(sz pos, solc_ast_t *stmt_ast);
void solc_ast_stmt_defer_destroy(solc_ast_t *defer_ast);
string_t *solc_ast_stmt_defer_build_tree(solc_ast_t *defer_ast);

solc_ast_t *solc_ast_stmt_if_create(sz pos, solc_ast_t *condition_expr_ast,
                                    solc_ast_t *stmt_ast, solc_ast_t *else_ast);
void solc_ast_stmt_if_destroy(solc_ast_t *if_ast);
string_t *solc_ast_stmt_if_build_tree(solc_ast_t *if_ast);

solc_ast_t *solc_ast_stmt_else_create(sz pos, solc_ast_t *stmt_ast);
void solc_ast_stmt_else_destroy(solc_ast_t *else_ast);
string_t *solc_ast_stmt_else_build_tree(solc_ast_t *else_ast);

#endif // __SOLC_AST_GROUP_STMT_H__
