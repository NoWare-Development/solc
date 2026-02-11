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
  SOLC_ASSUME(module_ast != nullptr);

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

sz solc_ast_import_to_string(char *buf, sz n, solc_ast_t *import_ast)
{
  SOLC_ASSUME(buf != nullptr && import_ast != nullptr &&
              import_ast->type == SOLC_AST_TYPE_NONE_IMPORT);

  SOLC_TODO("Import AST to string.");
  return n;
}
