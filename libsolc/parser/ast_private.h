#ifndef __SOLC_AST_PRIVATE_H__
#define __SOLC_AST_PRIVATE_H__

#include "containers/string.h"
#include "parser/ast/ast_group_expr_operand.h"
#include "parser/ast/ast_group_generic.h"
#include "parser/ast/ast_group_initlist.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast/ast_group_stmt.h"
#include "parser/ast/ast_group_type.h"
#include "parser/ast/ast_group_var.h"
#include "solc/parser/ast.h"

#define SOLC_AST(_name) (solc_ast_t *)(_name)
#define SOLC_AST_HEADER solc_ast_t header
#define SOLC_AST_INIT_HEADER(_ast_ptr, _pos, _type) \
  {                                                 \
    (_ast_ptr)->header.token_pos = (_pos);          \
    (_ast_ptr)->header.type = (_type);              \
  }
#define SOLC_AST_CAST(_name, _rawptr, _type) _type *_name = (_type *)(_rawptr)

typedef string_t *(*solc_ast_build_tree_func_t)(solc_ast_t *ast);

solc_ast_build_tree_func_t ast_get_build_tree_func(solc_ast_type_t ast_type);

string_t *ast_build_tree(string_t *heading, string_t **children_vs_v);

#endif // __SOLC_AST_PRIVATE_H__
