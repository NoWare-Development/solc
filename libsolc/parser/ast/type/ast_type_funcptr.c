#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
  solc_ast_t *arg_list_ast;
} ast_type_funcptr_t;

solc_ast_t *solc_ast_type_funcptr_create(sz pos, solc_ast_t *type_ast,
                                         solc_ast_t *arg_list_ast)
{
  SOLC_ASSUME(arg_list_ast != nullptr);
  ast_type_funcptr_t *out_funcptr_type = malloc(sizeof(ast_type_funcptr_t));
  SOLC_AST_INIT_HEADER(out_funcptr_type, pos, SOLC_AST_TYPE_TYPE_FUNCPTR);
  out_funcptr_type->type_ast = type_ast;
  out_funcptr_type->arg_list_ast = arg_list_ast;
  return SOLC_AST(out_funcptr_type);
}

void solc_ast_type_funcptr_destroy(solc_ast_t *funcptr_type_ast)
{
  SOLC_ASSUME(funcptr_type_ast != nullptr &&
              funcptr_type_ast->type == SOLC_AST_TYPE_TYPE_FUNCPTR);
  SOLC_AST_CAST(funcptr_type_data, funcptr_type_ast, ast_type_funcptr_t);
  SOLC_ASSUME(funcptr_type_data->arg_list_ast != nullptr);
  if (funcptr_type_data->type_ast != nullptr) {
    solc_ast_destroy(funcptr_type_data->type_ast);
  }
  solc_ast_destroy(funcptr_type_data->arg_list_ast);
  free(funcptr_type_data);
}

sz solc_ast_type_funcptr_to_string(char *buf, sz n,
                                   solc_ast_t *funcptr_type_ast)
{
  SOLC_ASSUME(buf != nullptr && funcptr_type_ast != nullptr &&
              funcptr_type_ast->type == SOLC_AST_TYPE_TYPE_FUNCPTR);

  SOLC_TODO("Type function pointer to string.");

  return 0;
}
