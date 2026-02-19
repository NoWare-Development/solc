#ifndef __SOLC_AST_OPERATOR_TYPES_H__
#define __SOLC_AST_OPERATOR_TYPES_H__

#include <solc/defs.h>

typedef enum {
  EXPR_OPERATOR_GROUP_BINARY,
  EXPR_OPERATOR_GROUP_COMPARE,
  EXPR_OPERATOR_GROUP_BOOLEAN,
  EXPR_OPERATOR_GROUP_ASSIGN,
  EXPR_OPERATOR_GROUP_PREFIX,
} expr_operator_group_t;

#define __EXPR_OPERATOR_TYPE_FULL_NAME(type_name, group_name) \
  __SOLC_CONCAT(EXPR_OPERATOR_TYPE_,                          \
                __SOLC_CONCAT(group_name, __SOLC_CONCAT(_, type_name)))

#define __EXPR_OPERATORS   \
  __EXPR_OPERATORS_BINARY  \
  __EXPR_OPERATORS_COMPARE \
  __EXPR_OPERATORS_BOOLEAN \
  __EXPR_OPERATORS_ASSIGN  \
  __EXPR_OPERATORS_PREFIX

#define __EXPR_OPERATORS_BINARY            \
  __EXPR_OPERATORS_X(ADD, BINARY, 0, "+")  \
  __EXPR_OPERATORS_X(SUB, BINARY, 1, "-")  \
  __EXPR_OPERATORS_X(MUL, BINARY, 2, "*")  \
  __EXPR_OPERATORS_X(DIV, BINARY, 3, "/")  \
  __EXPR_OPERATORS_X(MOD, BINARY, 4, "%")  \
  __EXPR_OPERATORS_X(SHL, BINARY, 5, "<<") \
  __EXPR_OPERATORS_X(SHR, BINARY, 6, ">>") \
  __EXPR_OPERATORS_X(AND, BINARY, 7, "&")  \
  __EXPR_OPERATORS_X(OR, BINARY, 8, "|")   \
  __EXPR_OPERATORS_X(XOR, BINARY, 9, "^")

#define __EXPR_OPERATORS_COMPARE                \
  __EXPR_OPERATORS_X(EQ, COMPARE, 0, "==")      \
  __EXPR_OPERATORS_X(NOTEQ, COMPARE, 1, "!=")   \
  __EXPR_OPERATORS_X(LTHAN, COMPARE, 2, "<")    \
  __EXPR_OPERATORS_X(GTHAN, COMPARE, 3, ">")    \
  __EXPR_OPERATORS_X(LTHANEQ, COMPARE, 4, "<=") \
  __EXPR_OPERATORS_X(GTHANEQ, COMPARE, 5, ">=")

#define __EXPR_OPERATORS_BOOLEAN            \
  __EXPR_OPERATORS_X(AND, BOOLEAN, 0, "&&") \
  __EXPR_OPERATORS_X(OR, BOOLEAN, 1, "||")

#define __EXPR_OPERATORS_ASSIGN               \
  __EXPR_OPERATORS_X(EQ, ASSIGN, 0, "=")      \
  __EXPR_OPERATORS_X(ADDEQ, ASSIGN, 1, "+=")  \
  __EXPR_OPERATORS_X(SUBEQ, ASSIGN, 2, "-=")  \
  __EXPR_OPERATORS_X(MULEQ, ASSIGN, 3, "*=")  \
  __EXPR_OPERATORS_X(DIVEQ, ASSIGN, 4, "/=")  \
  __EXPR_OPERATORS_X(MODEQ, ASSIGN, 5, "%=")  \
  __EXPR_OPERATORS_X(SHLEQ, ASSIGN, 6, "<<=") \
  __EXPR_OPERATORS_X(SHREQ, ASSIGN, 7, ">>=") \
  __EXPR_OPERATORS_X(ANDEQ, ASSIGN, 8, "&=")  \
  __EXPR_OPERATORS_X(OREQ, ASSIGN, 9, "|=")   \
  __EXPR_OPERATORS_X(XOREQ, ASSIGN, 10, "^=")

#define __EXPR_OPERATORS_PREFIX             \
  __EXPR_OPERATORS_X(NOT, PREFIX, 0, "!")   \
  __EXPR_OPERATORS_X(BNOT, PREFIX, 1, "~")  \
  __EXPR_OPERATORS_X(NEG, PREFIX, 2, "-")   \
  __EXPR_OPERATORS_X(DEREF, PREFIX, 3, "*") \
  __EXPR_OPERATORS_X(ADDRESS, PREFIX, 4, "&")

typedef enum {
#define __EXPR_OPERATORS_X(type_name, group_name, id, display_val) \
  __EXPR_OPERATOR_TYPE_FULL_NAME(type_name, group_name) =          \
    (((u8)(EXPR_OPERATOR_GROUP_##group_name) & 0xFF) << 8) | ((id) & 0xFF),
  __EXPR_OPERATORS
#undef __EXPR_OPERATORS_X
} expr_operator_type_t;

expr_operator_group_t expr_operator_type_get_group(expr_operator_type_t type);
const char *ast_expr_operator_type_to_string(expr_operator_type_t t);

#endif // __SOLC_AST_OPERATOR_TYPES_H__
