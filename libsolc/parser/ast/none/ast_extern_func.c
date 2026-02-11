#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"

solc_ast_t *solc_ast_extern_func_create(solc_ast_t *func_ast)
{
  SOLC_ASSUME(func_ast != nullptr && func_ast->type == SOLC_AST_TYPE_NONE_FUNC);
  func_ast->type = SOLC_AST_TYPE_NONE_EXTERN_FUNC;
  return func_ast;
}

void solc_ast_extern_func_destroy(solc_ast_t *extern_func_ast)
{
  SOLC_ASSUME(extern_func_ast != nullptr &&
              extern_func_ast->type == SOLC_AST_TYPE_NONE_EXTERN_FUNC);
  extern_func_ast->type = SOLC_AST_TYPE_NONE_FUNC;
  solc_ast_func_destroy(extern_func_ast);
}

sz solc_ast_extern_func_to_string(char *buf, sz n, solc_ast_t *extern_func_ast)
{
  SOLC_ASSUME(buf != nullptr && extern_func_ast != nullptr &&
              extern_func_ast->type == SOLC_AST_TYPE_NONE_EXTERN_FUNC);

  SOLC_TODO("Extern function to string.");
  return 0;
}
