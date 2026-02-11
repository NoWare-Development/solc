#include "parser/ast_private.h"
#include "solc/parser/ast.h"

solc_ast_t *solc_ast_func_explicit_create(sz pos, solc_ast_t *func_ast)
{
  SOLC_ASSUME(func_ast != nullptr && func_ast->type == SOLC_AST_TYPE_NONE_FUNC);
  func_ast->type = SOLC_AST_TYPE_NONE_FUNC_EXPLICIT;
  return func_ast;
}

void solc_ast_func_explicit_destroy(solc_ast_t *func_explicit_ast)
{
  SOLC_ASSUME(func_explicit_ast != nullptr &&
              func_explicit_ast->type == SOLC_AST_TYPE_NONE_FUNC_EXPLICIT);
  func_explicit_ast->type = SOLC_AST_TYPE_NONE_FUNC;
  solc_ast_func_destroy(func_explicit_ast);
}

sz solc_ast_func_explicit_to_string(char *buf, sz n,
                                    solc_ast_t *func_explicit_ast)
{
  SOLC_ASSUME(buf != nullptr && func_explicit_ast != nullptr &&
              func_explicit_ast->type == SOLC_AST_TYPE_NONE_FUNC_EXPLICIT);

  SOLC_TODO("Explicit function to string.");

  return 0;
}
