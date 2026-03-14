#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
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
  solc_ast_destroy_if_exists(module_data->submodule_ast);
  free(module_ast);
}

string_t *solc_ast_module_build_tree(solc_ast_t *module_ast)
{
  SOLC_ASSUME(module_ast != nullptr &&
              module_ast->type == SOLC_AST_TYPE_NONE_MODULE);
  SOLC_AST_CAST(module_data, module_ast, ast_module_t);
  SOLC_ASSUME(module_data->name != nullptr);
  string_t header = string_create_from("MODULE { name: \"");
  string_append_cstr(&header, module_data->name);
  string_append_cstr(&header, "\" }");

  if (module_data->submodule_ast == nullptr) {
    string_t *out_v = vector_reserve(string_t, 1);
    vector_push(out_v, header);
    return out_v;
  }

  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v, module_data->submodule_ast);
  return ast_build_tree(&header, children_vs_v);
}
