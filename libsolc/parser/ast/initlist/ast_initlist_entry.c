#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
} ast_initlist_entry_t;

solc_ast_t *solc_ast_initlist_entry_create(sz pos, solc_ast_t *expr_ast)
{
  SOLC_ASSUME(expr_ast != nullptr);
  ast_initlist_entry_t *out_initlist_entry =
    malloc(sizeof(ast_initlist_entry_t));
  SOLC_AST_INIT_HEADER(out_initlist_entry, pos, SOLC_AST_TYPE_INITLIST_ENTRY);
  out_initlist_entry->expr_ast = expr_ast;
  return SOLC_AST(out_initlist_entry);
}

void solc_ast_initlist_entry_destroy(solc_ast_t *initlist_entry_ast)
{
  SOLC_ASSUME(initlist_entry_ast != nullptr &&
              initlist_entry_ast->type == SOLC_AST_TYPE_INITLIST_ENTRY);
  SOLC_AST_CAST(initlist_entry_data, initlist_entry_ast, ast_initlist_entry_t);
  SOLC_ASSUME(initlist_entry_data->expr_ast != nullptr);
  solc_ast_destroy(initlist_entry_data->expr_ast);
  free(initlist_entry_ast);
}

sz solc_ast_initlist_entry_to_string(char *buf, sz n,
                                     solc_ast_t *initlist_entry_ast)
{
  SOLC_ASSUME(buf != nullptr && initlist_entry_ast != nullptr &&
              initlist_entry_ast->type == SOLC_AST_TYPE_INITLIST_ENTRY);

  SOLC_TODO("Initialization list entry to string.");

  return 0;
}
