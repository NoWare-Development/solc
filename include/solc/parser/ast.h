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

#define __SOLC_AST_TYPES_NONE               \
  __SOLC_AST_TYPE_X(ERR, NONE, 0)           \
  __SOLC_AST_TYPE_X(ROOT, NONE, 1)          \
  __SOLC_AST_TYPE_X(EXPR, NONE, 2)          \
  __SOLC_AST_TYPE_X(MODULE, NONE, 3)        \
  __SOLC_AST_TYPE_X(IMPORT, NONE, 4)        \
  __SOLC_AST_TYPE_X(TYPEDEF, NONE, 5)       \
  __SOLC_AST_TYPE_X(EXPORTED_FUNC, NONE, 6) \
  __SOLC_AST_TYPE_X(EXTERN_FUNC, NONE, 7)   \
  __SOLC_AST_TYPE_X(QUALIFIER, NONE, 8)     \
  __SOLC_AST_TYPE_X(NONE, NONE, 9)          \
  __SOLC_AST_TYPE_X(LABEL, NONE, 10)        \
  __SOLC_AST_TYPE_X(VARIADIC, NONE, 11)     \
  __SOLC_AST_TYPE_X(STRUCT, NONE, 12)       \
  __SOLC_AST_TYPE_X(UNION, NONE, 13)        \
  __SOLC_AST_TYPE_X(ENUM, NONE, 14)         \
  __SOLC_AST_TYPE_X(ENUM_ELEMENT, NONE, 15) \
  __SOLC_AST_TYPE_X(FUNC, NONE, 16)         \
  __SOLC_AST_TYPE_X(FUNC_ARGLIST, NONE, 17) \
  __SOLC_AST_TYPE_X(NAMESPACE, NONE, 18)    \
  __SOLC_AST_TYPE_X(INITLIST, NONE, 19)     \
  __SOLC_AST_TYPE_X(PREFIX_EXPR, NONE, 20)  \
  __SOLC_AST_TYPE_X(FUNC_EXPLICIT, NONE, 21)

#define __SOLC_AST_TYPES_STMT          \
  __SOLC_AST_TYPE_X(BLOCK, STMT, 0)    \
  __SOLC_AST_TYPE_X(RETURN, STMT, 1)   \
  __SOLC_AST_TYPE_X(GOTO, STMT, 2)     \
  __SOLC_AST_TYPE_X(LABEL, STMT, 3)    \
  __SOLC_AST_TYPE_X(BREAK, STMT, 4)    \
  __SOLC_AST_TYPE_X(CONTINUE, STMT, 5) \
  __SOLC_AST_TYPE_X(WHILE, STMT, 6)    \
  __SOLC_AST_TYPE_X(FOR, STMT, 7)      \
  __SOLC_AST_TYPE_X(DOWHILE, STMT, 8)  \
  __SOLC_AST_TYPE_X(EXPR, STMT, 9)     \
  __SOLC_AST_TYPE_X(SWITCH, STMT, 10)  \
  __SOLC_AST_TYPE_X(CASE, STMT, 11)    \
  __SOLC_AST_TYPE_X(DEFAULT, STMT, 12) \
  __SOLC_AST_TYPE_X(DEFER, STMT, 13)   \
  __SOLC_AST_TYPE_X(IF, STMT, 14)      \
  __SOLC_AST_TYPE_X(ELSE, STMT, 15)

#define __SOLC_AST_TYPES_TYPE         \
  __SOLC_AST_TYPE_X(PLAIN, TYPE, 0)   \
  __SOLC_AST_TYPE_X(ARRAY, TYPE, 1)   \
  __SOLC_AST_TYPE_X(POINTER, TYPE, 2) \
  __SOLC_AST_TYPE_X(FUNCPTR, TYPE, 3) \
  __SOLC_AST_TYPE_X(TYPEOF, TYPE, 4)

#define __SOLC_AST_TYPES_VAR      \
  __SOLC_AST_TYPE_X(DECL, VAR, 0) \
  __SOLC_AST_TYPE_X(DEF, VAR, 1)

#define __SOLC_AST_TYPES_EXPR_OPERAND                \
  __SOLC_AST_TYPE_X(VOID, EXPR_OPERAND, 0)           \
  __SOLC_AST_TYPE_X(IDENTIFIER, EXPR_OPERAND, 1)     \
  __SOLC_AST_TYPE_X(NUM, EXPR_OPERAND, 2)            \
  __SOLC_AST_TYPE_X(NUMFLOAT, EXPR_OPERAND, 3)       \
  __SOLC_AST_TYPE_X(ARRAY_ELEMENT, EXPR_OPERAND, 4)  \
  __SOLC_AST_TYPE_X(CAST_TO, EXPR_OPERAND, 5)        \
  __SOLC_AST_TYPE_X(CALL, EXPR_OPERAND, 6)           \
  __SOLC_AST_TYPE_X(NUM_TYPESPEC, EXPR_OPERAND, 7)   \
  __SOLC_AST_TYPE_X(STRING, EXPR_OPERAND, 8)         \
  __SOLC_AST_TYPE_X(SYMBOL, EXPR_OPERAND, 9)         \
  __SOLC_AST_TYPE_X(ACCESS_MEMBER, EXPR_OPERAND, 10) \
  __SOLC_AST_TYPE_X(GENERIC_CALL, EXPR_OPERAND, 11)  \
  __SOLC_AST_TYPE_X(SIZEOF, EXPR_OPERAND, 12)

#define __SOLC_AST_TYPES_INITLIST                \
  __SOLC_AST_TYPE_X(ENTRY, INITLIST, 0)          \
  __SOLC_AST_TYPE_X(ENTRY_EXPLICIT, INITLIST, 1) \
  __SOLC_AST_TYPE_X(ENTRY_EXPLICIT_ARRAY_ELEMENT, INITLIST, 2)

#define __SOLC_AST_TYPES_GENERIC                       \
  __SOLC_AST_TYPE_X(STRUCT, GENERIC, 0)                \
  __SOLC_AST_TYPE_X(FUNC, GENERIC, 1)                  \
  __SOLC_AST_TYPE_X(PLACEHOLDER_TYPE_LIST, GENERIC, 2) \
  __SOLC_AST_TYPE_X(PLACEHOLDER_TYPE, GENERIC, 3)      \
  __SOLC_AST_TYPE_X(TYPE_LIST, GENERIC, 4)             \
  __SOLC_AST_TYPE_X(TYPE, GENERIC, 5)                  \
  __SOLC_AST_TYPE_X(NAMESPACE, GENERIC, 6)             \
  __SOLC_AST_TYPE_X(FUNC_EXPLICIT, GENERIC, 7)

typedef enum {
#define __SOLC_AST_TYPE_X(type_name, group_name, in_group_id) \
  __SOLC_FULL_AST_NAME(type_name, group_name) =               \
    (((u8)(SOLC_AST_GROUP_##group_name) & 0xFF) << 8) |       \
    ((in_group_id) & 0xFF),
  __SOLC_AST_TYPES
#undef __SOLC_AST_TYPE_X
} solc_ast_type_t;

typedef struct {
  sz token_pos;
  solc_ast_type_t type;
} solc_ast_t;

typedef void (*solc_ast_destroy_func_t)(solc_ast_t *ast);
typedef sz (*solc_ast_to_string_func_t)(char *buf, sz n, solc_ast_t *ast);

solc_ast_destroy_func_t solc_ast_get_destroy_func(solc_ast_type_t ast_type);
solc_ast_to_string_func_t solc_ast_get_to_string_func(solc_ast_type_t ast_type);

#define solc_ast_destroy(ast) \
  solc_ast_get_destroy_func(((solc_ast_t *)(ast))->type)((solc_ast_t *)(ast))
#define solc_ast_to_string(buf, n, ast) \
  solc_ast_get_to_string_func(          \
    ((solc_ast_t *)(ast))->type)((buf), (n), (solc_ast_t *)(ast))

#endif // __SOLC_AST_H__
