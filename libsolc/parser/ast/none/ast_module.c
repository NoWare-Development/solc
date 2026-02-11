#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *submodule_ast;
  char *name;
} ast_module_t;

solc_ast_t *solc_ast_module_create(sz pos, const char *name,
                                   solc_ast_t *submodule_ast)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_module_t *out_module = malloc(sizeof(ast_module_t) + name_len);
  SOLC_AST_INIT_HEADER(out_module, pos, SOLC_AST_TYPE_NONE_MODULE);
  out_module->submodule_ast = submodule_ast;
  out_module->name = (char *)out_module + sizeof(ast_module_t);
  memcpy(out_module->name, name, name_len);
  return SOLC_AST(out_module);
}

void solc_ast_module_destroy(solc_ast_t *module_ast)
{
  SOLC_ASSUME(module_ast != nullptr &&
              module_ast->type == SOLC_AST_TYPE_NONE_MODULE);
  SOLC_AST_CAST(module_data, module_ast, ast_module_t);
  if (module_data->submodule_ast != nullptr) {
    solc_ast_destroy(module_data->submodule_ast);
  }
  free(module_ast);
}

sz solc_ast_module_to_string(char *buf, sz n, solc_ast_t *module_ast)
{
  SOLC_ASSUME(buf != nullptr && module_ast != nullptr &&
              module_ast->type == SOLC_AST_TYPE_NONE_MODULE);

  SOLC_AST_CAST(module_data, module_ast, ast_module_t);
  SOLC_ASSUME(module_data->name != nullptr);

  if (module_data->submodule_ast != nullptr) {
    SOLC_TODO("Module with submodule to string.");
  }

  return snprintf(buf, n, "MODULE { name: \"%s\" }", module_data->name);
}
