#include "solc/parser/ast.h"
#include "solc/defs.h"

#include "parser/ast_private.h"

solc_ast_destroy_func_t solc_ast_get_destroy_func(solc_ast_type_t ast_type)
{
  // TODO: More AST types.
  switch (ast_type) {
  case SOLC_AST_TYPE_NONE_ERR:
    return solc_ast_err_destroy;

  case SOLC_AST_TYPE_NONE_ROOT:
    return solc_ast_root_destroy;

  case SOLC_AST_TYPE_NONE_EXPR:
    return solc_ast_expr_destroy;

  default:
    SOLC_NOREACH();
  }
}
