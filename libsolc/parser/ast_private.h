#ifndef __SOLC_AST_PRIVATE_H__
#define __SOLC_AST_PRIVATE_H__

#include "parser/ast/ast_group_expr_operand.h"
#include "parser/ast/ast_group_generic.h"
#include "parser/ast/ast_group_initlist.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast/ast_group_stmt.h"
#include "parser/ast/ast_group_type.h"
#include "parser/ast/ast_group_var.h"

#define SOLC_AST(_name) (solc_ast_t *)(_name)
#define SOLC_AST_HEADER solc_ast_t header
#define SOLC_AST_INIT_HEADER(_ast_ptr, _pos, _type) \
  {                                                 \
    (_ast_ptr)->header.token_pos = (_pos);          \
    (_ast_ptr)->header.type = (_type);              \
  }
#define SOLC_AST_CAST(_name, _rawptr, _type) _type *_name = (_type *)(_rawptr)

#endif // __SOLC_AST_PRIVATE_H__
