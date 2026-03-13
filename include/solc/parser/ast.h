#ifndef __SOLC_AST_H__
#define __SOLC_AST_H__

#include <solc/defs.h>

typedef enum {
  SOLC_AST_GROUP_NONE = 0,
  SOLC_AST_GROUP_STMT,
  SOLC_AST_GROUP_TYPE,
  SOLC_AST_GROUP_VAR,
  SOLC_AST_GROUP_EXPR_OPERAND,
  SOLC_AST_GROUP_INITLIST,
  SOLC_AST_GROUP_GENERIC,
} solc_ast_group_t;

#define __SOLC_FULL_AST_NAME(type_name, group_name) \
  __SOLC_CONCAT(SOLC_AST_TYPE_,                     \
                __SOLC_CONCAT(group_name, __SOLC_CONCAT(_, type_name)))

// A bunch of X macros which (in this example) will be expanded to something
// like that:
// SOLC_AST_TYPE_STMT_BLOCK = (((u8)(SOLC_AST_GROUP_STMT) & 0xFF) << 8)
//                            | ((id) & 0xFF)

// #define __SOLC_AST_TYPE_X(type_name, group_name, in_group_id) ...
#define __SOLC_AST_TYPES        \
  __SOLC_AST_TYPES_NONE         \
  __SOLC_AST_TYPES_STMT         \
  __SOLC_AST_TYPES_TYPE         \
  __SOLC_AST_TYPES_VAR          \
  __SOLC_AST_TYPES_EXPR_OPERAND \
  __SOLC_AST_TYPES_INITLIST     \
  __SOLC_AST_TYPES_GENERIC

#define __SOLC_AST_TYPES_NONE                                \
  __SOLC_AST_TYPE_X(ERR, NONE, 0, err)                       \
  __SOLC_AST_TYPE_X(ROOT, NONE, 1, root)                     \
  __SOLC_AST_TYPE_X(EXPR, NONE, 2, expr)                     \
  __SOLC_AST_TYPE_X(MODULE, NONE, 3, module)                 \
  __SOLC_AST_TYPE_X(IMPORT, NONE, 4, import)                 \
  __SOLC_AST_TYPE_X(TYPEDEF, NONE, 5, typedef)               \
  __SOLC_AST_TYPE_X(EXTERN_FUNC, NONE, 6, extern_func)       \
  __SOLC_AST_TYPE_X(EXTERN_VARDECL, NONE, 7, extern_vardecl) \
  __SOLC_AST_TYPE_X(QUALIFIER, NONE, 8, qualifier)           \
  __SOLC_AST_TYPE_X(NONE, NONE, 9, none)                     \
  __SOLC_AST_TYPE_X(VARIADIC, NONE, 10, variadic)            \
  __SOLC_AST_TYPE_X(STRUCT, NONE, 11, struct)                \
  __SOLC_AST_TYPE_X(UNION, NONE, 12, union)                  \
  __SOLC_AST_TYPE_X(ENUM, NONE, 13, enum)                    \
  __SOLC_AST_TYPE_X(ENUM_ELEMENT, NONE, 14, enum_element)    \
  __SOLC_AST_TYPE_X(FUNC, NONE, 15, func)                    \
  __SOLC_AST_TYPE_X(FUNC_ARGLIST, NONE, 16, func_arglist)    \
  __SOLC_AST_TYPE_X(NAMESPACE, NONE, 17, namespace)          \
  __SOLC_AST_TYPE_X(INITLIST, NONE, 18, initlist)            \
  __SOLC_AST_TYPE_X(PREFIX_EXPR, NONE, 19, prefix_expr)

#define __SOLC_AST_TYPES_STMT                               \
  __SOLC_AST_TYPE_X(BLOCK, STMT, 0, stmt_block)             \
  __SOLC_AST_TYPE_X(RETURN, STMT, 1, stmt_return)           \
  __SOLC_AST_TYPE_X(GOTO, STMT, 2, stmt_goto)               \
  __SOLC_AST_TYPE_X(LABEL, STMT, 3, stmt_label)             \
  __SOLC_AST_TYPE_X(BREAK, STMT, 4, stmt_break)             \
  __SOLC_AST_TYPE_X(CONTINUE, STMT, 5, stmt_continue)       \
  __SOLC_AST_TYPE_X(FALLTHROUGH, STMT, 6, stmt_fallthrough) \
  __SOLC_AST_TYPE_X(WHILE, STMT, 7, stmt_while)             \
  __SOLC_AST_TYPE_X(FOR, STMT, 8, stmt_for)                 \
  __SOLC_AST_TYPE_X(DOWHILE, STMT, 9, stmt_dowhile)         \
  __SOLC_AST_TYPE_X(EXPR, STMT, 10, stmt_expr)              \
  __SOLC_AST_TYPE_X(SWITCH, STMT, 11, stmt_switch)          \
  __SOLC_AST_TYPE_X(CASE, STMT, 12, stmt_case)              \
  __SOLC_AST_TYPE_X(DEFAULT, STMT, 13, stmt_default)        \
  __SOLC_AST_TYPE_X(DEFER, STMT, 14, stmt_defer)            \
  __SOLC_AST_TYPE_X(IF, STMT, 15, stmt_if)                  \
  __SOLC_AST_TYPE_X(ELSE, STMT, 16, stmt_else)

#define __SOLC_AST_TYPES_TYPE                       \
  __SOLC_AST_TYPE_X(PLAIN, TYPE, 0, type_plain)     \
  __SOLC_AST_TYPE_X(ARRAY, TYPE, 1, type_array)     \
  __SOLC_AST_TYPE_X(POINTER, TYPE, 2, type_pointer) \
  __SOLC_AST_TYPE_X(FUNCPTR, TYPE, 3, type_funcptr) \
  __SOLC_AST_TYPE_X(TYPEOF, TYPE, 4, type_typeof)

#define __SOLC_AST_TYPES_VAR                \
  __SOLC_AST_TYPE_X(DECL, VAR, 0, var_decl) \
  __SOLC_AST_TYPE_X(DEF, VAR, 1, var_def)

#define __SOLC_AST_TYPES_EXPR_OPERAND                                          \
  __SOLC_AST_TYPE_X(VOID, EXPR_OPERAND, 0, expr_operand_void)                  \
  __SOLC_AST_TYPE_X(IDENTIFIER, EXPR_OPERAND, 1, expr_operand_identifier)      \
  __SOLC_AST_TYPE_X(NUM, EXPR_OPERAND, 2, expr_operand_num)                    \
  __SOLC_AST_TYPE_X(NUMFLOAT, EXPR_OPERAND, 3, expr_operand_numfloat)          \
  __SOLC_AST_TYPE_X(ARRAY_ELEMENT, EXPR_OPERAND, 4,                            \
                    expr_operand_array_element)                                \
  __SOLC_AST_TYPE_X(CAST_TO, EXPR_OPERAND, 5, expr_operand_cast_to)            \
  __SOLC_AST_TYPE_X(CALL, EXPR_OPERAND, 6, expr_operand_call)                  \
  __SOLC_AST_TYPE_X(STRING, EXPR_OPERAND, 7, expr_operand_string)              \
  __SOLC_AST_TYPE_X(SYMBOL, EXPR_OPERAND, 8, expr_operand_symbol)              \
  __SOLC_AST_TYPE_X(ACCESS_MEMBER, EXPR_OPERAND, 9,                            \
                    expr_operand_access_member)                                \
  __SOLC_AST_TYPE_X(GENERIC_CALL, EXPR_OPERAND, 10, expr_operand_generic_call) \
  __SOLC_AST_TYPE_X(SIZEOF, EXPR_OPERAND, 11, expr_operand_sizeof)

#define __SOLC_AST_TYPES_INITLIST                                         \
  __SOLC_AST_TYPE_X(ENTRY, INITLIST, 0, initlist_entry)                   \
  __SOLC_AST_TYPE_X(ENTRY_EXPLICIT, INITLIST, 1, initlist_entry_explicit) \
  __SOLC_AST_TYPE_X(ENTRY_EXPLICIT_ARRAY_ELEMENT, INITLIST, 2,            \
                    initlist_entry_explicit_array_element)

#define __SOLC_AST_TYPES_GENERIC                                            \
  __SOLC_AST_TYPE_X(STRUCT, GENERIC, 0, generic_struct)                     \
  __SOLC_AST_TYPE_X(FUNC, GENERIC, 1, generic_func)                         \
  __SOLC_AST_TYPE_X(PLACEHOLDER_TYPE_LIST, GENERIC, 2,                      \
                    generic_placeholder_type_list)                          \
  __SOLC_AST_TYPE_X(PLACEHOLDER_TYPE, GENERIC, 3, generic_placeholder_type) \
  __SOLC_AST_TYPE_X(TYPE_LIST, GENERIC, 4, generic_type_list)               \
  __SOLC_AST_TYPE_X(TYPE, GENERIC, 5, generic_type)                         \
  __SOLC_AST_TYPE_X(NAMESPACE, GENERIC, 6, generic_namespace)

typedef enum {
#define __SOLC_AST_TYPE_X(type_name, group_name, in_group_id, in_code_name) \
  __SOLC_FULL_AST_NAME(type_name, group_name) =                             \
    (((u8)(SOLC_AST_GROUP_##group_name) & 0xFF) << 8) |                     \
    ((in_group_id) & 0xFF),
  __SOLC_AST_TYPES
#undef __SOLC_AST_TYPE_X
} solc_ast_type_t;

typedef struct {
  sz token_pos;
  solc_ast_type_t type : 16;
} solc_ast_t;

typedef void (*solc_ast_destroy_func_t)(solc_ast_t *ast);

solc_ast_group_t solc_ast_type_get_group(solc_ast_type_t type);
u8 solc_ast_type_get_id_in_group(solc_ast_type_t type);

solc_ast_destroy_func_t solc_ast_get_destroy_func(solc_ast_type_t ast_type);
void solc_ast_to_string(char *buf, sz n, solc_ast_t *ast);

#define solc_ast_destroy(ast) \
  solc_ast_get_destroy_func(((solc_ast_t *)(ast))->type)((solc_ast_t *)(ast))

#endif // __SOLC_AST_H__
