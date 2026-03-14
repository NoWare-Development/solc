#ifndef __SOLC_AST_FUNC_TYPE_H__
#define __SOLC_AST_FUNC_TYPE_H__

typedef enum {
  SOLC_AST_FUNC_TYPE_DEFAULT = 0,
  SOLC_AST_FUNC_TYPE_EXPLICIT = 1,
  SOLC_AST_FUNC_TYPE_EXPORTED = 2,
} solc_ast_func_type_t;

const char *solc_ast_func_type_to_string(solc_ast_func_type_t func_type);

#endif // __SOLC_AST_FUNC_TYPE_H__
