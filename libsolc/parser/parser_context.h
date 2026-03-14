#ifndef __SOLC_PARSER_CONTEXT_H__
#define __SOLC_PARSER_CONTEXT_H__

#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

typedef solc_ast_t *(*parser_toplevel_func_t)(solc_parser_t *);
typedef solc_ast_t *(*parser_stmt_func_t)(solc_parser_t *);
typedef solc_ast_t *(*parser_struct_func_t)(solc_parser_t *);
typedef solc_ast_t *(*parser_union_func_t)(solc_parser_t *);

void parser_context_initialize(void);

parser_toplevel_func_t parser_context_get_toplevel_func(const char *str);
parser_stmt_func_t parser_context_get_stmt_func(const char *str);
parser_struct_func_t parser_context_get_struct_func(const char *str);
parser_union_func_t parser_context_get_union_func(const char *str);

b8 parser_context_is_qualifier(const char *str);

#endif // __SOLC_PARSER_CONTEXT_H__
