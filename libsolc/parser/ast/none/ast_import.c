#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *module_ast;
} ast_import_t;

solc_ast_t *solc_ast_import_create(sz pos, solc_ast_t *module_ast)
{
  SOLC_ASSUME(module_ast != nullptr &&
              module_ast->type == SOLC_AST_TYPE_NONE_MODULE);

  ast_import_t *out_import = malloc(sizeof(ast_import_t));
  SOLC_AST_INIT_HEADER(out_import, pos, SOLC_AST_TYPE_NONE_IMPORT);
  out_import->module_ast = module_ast;
  return SOLC_AST(out_import);
}

void solc_ast_import_destroy(solc_ast_t *import_ast)
{
  SOLC_ASSUME(import_ast != nullptr &&
              import_ast->type == SOLC_AST_TYPE_NONE_IMPORT);

  SOLC_AST_CAST(import_data, import_ast, ast_import_t);
  SOLC_ASSUME(import_data->module_ast != nullptr);

  solc_ast_destroy(import_data->module_ast);

  free(import_ast);
}

string_t *solc_ast_import_build_tree(solc_ast_t *import_ast)
{
  SOLC_ASSUME(import_ast != nullptr &&
              import_ast->type == SOLC_AST_TYPE_NONE_IMPORT);
  SOLC_AST_CAST(import_data, import_ast, ast_import_t);
  SOLC_ASSUME(import_data->module_ast != nullptr &&
              import_data->module_ast->type == SOLC_AST_TYPE_NONE_MODULE);

  string_t header = string_create_from("IMPORT");
  string_t **children_vs_v = vector_reserve(string_t *, 1);
  vector_push(children_vs_v,
              solc_ast_module_build_tree(import_data->module_ast));
  return ast_build_tree(&header, children_vs_v);
}
