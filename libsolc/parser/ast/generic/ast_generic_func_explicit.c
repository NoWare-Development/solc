#include "parser/ast_private.h"
#include "solc/parser/ast.h"

solc_ast_t *solc_ast_generic_func_explicit_create(sz pos,
                                                  solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  generic_func_ast->type = SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT;
  return generic_func_ast;
}

void solc_ast_generic_func_explicit_destroy(
  solc_ast_t *generic_func_explicit_ast)
{
  SOLC_ASSUME(generic_func_explicit_ast != nullptr &&
              generic_func_explicit_ast->type ==
                SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT);
  generic_func_explicit_ast->type = SOLC_AST_TYPE_GENERIC_FUNC;
  solc_ast_generic_func_destroy(generic_func_explicit_ast);
}

sz solc_ast_generic_func_explicit_to_string(
  char *buf, sz n, solc_ast_t *generic_func_explicit_ast)
{
  SOLC_ASSUME(buf != nullptr && generic_func_explicit_ast != nullptr &&
              generic_func_explicit_ast->type ==
                SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT);

  SOLC_TODO("Explicit generic function to string.");

  return 0;
}
