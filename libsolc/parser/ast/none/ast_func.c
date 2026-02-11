#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
  solc_ast_t *arg_list_ast;
  solc_ast_t *block_ast;
  char *name;
} ast_func_t;

solc_ast_t *solc_ast_func_create(sz pos, const char *name, solc_ast_t *type_ast,
                                 solc_ast_t *arg_list_ast,
                                 solc_ast_t *block_ast)
{
  SOLC_ASSUME(name != nullptr && arg_list_ast != nullptr &&
              block_ast != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_func_t *out_func = malloc(sizeof(ast_func_t) + name_len);
  SOLC_AST_INIT_HEADER(out_func, pos, SOLC_AST_TYPE_NONE_FUNC);
  out_func->type_ast = type_ast;
  out_func->arg_list_ast = arg_list_ast;
  out_func->block_ast = block_ast;
  out_func->name = (char *)out_func + sizeof(ast_func_t);
  memcpy(out_func->name, name, name_len);
  return SOLC_AST(out_func);
}

void solc_ast_func_destroy(solc_ast_t *func_ast)
{
  SOLC_ASSUME(func_ast != nullptr && func_ast->type == SOLC_AST_TYPE_NONE_FUNC);

  SOLC_AST_CAST(func_data, func_ast, ast_func_t);
  if (func_data->type_ast != nullptr) {
    solc_ast_destroy(func_data->type_ast);
  }

  solc_ast_destroy(func_data->arg_list_ast);
  solc_ast_destroy(func_data->block_ast);

  free(func_ast);
}

sz solc_ast_func_to_string(char *buf, sz n, solc_ast_t *func_ast)
{
  SOLC_ASSUME(buf != nullptr && func_ast != nullptr &&
              func_ast->type == SOLC_AST_TYPE_NONE_FUNC);

  SOLC_TODO("Function AST to string.");
  return n;
}
