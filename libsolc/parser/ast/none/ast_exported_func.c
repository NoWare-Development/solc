#include "parser/ast_private.h"
#include "solc/parser/ast.h"

solc_ast_t *solc_ast_exported_func_create(solc_ast_t *func_ast)
{
  SOLC_ASSUME(func_ast != nullptr && func_ast->type == SOLC_AST_TYPE_NONE_FUNC);
  func_ast->type = SOLC_AST_TYPE_NONE_EXPORTED_FUNC;
  return func_ast;
}

void solc_ast_exported_func_destroy(solc_ast_t *exported_func_ast)
{
  SOLC_ASSUME(exported_func_ast != nullptr &&
              exported_func_ast->type == SOLC_AST_TYPE_NONE_EXPORTED_FUNC);
  exported_func_ast->type = SOLC_AST_TYPE_NONE_FUNC;
  solc_ast_func_destroy(exported_func_ast);
}

sz solc_ast_exported_func_to_string(char *buf, sz n,
                                    solc_ast_t *exported_func_ast)
{
  SOLC_ASSUME(buf != nullptr && exported_func_ast != nullptr &&
              exported_func_ast->type == SOLC_AST_TYPE_NONE_EXPORTED_FUNC);

  SOLC_TODO("Exported function AST to string.");
  return 0;
}
