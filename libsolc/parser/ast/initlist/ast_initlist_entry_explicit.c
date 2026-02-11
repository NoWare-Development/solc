#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
  char *name;
} ast_initlist_entry_explicit_t;

solc_ast_t *solc_ast_initlist_entry_explicit_create(sz pos, const char *name,
                                                    solc_ast_t *expr_ast)
{
  SOLC_ASSUME(name != nullptr && expr_ast != nullptr);
  const sz name_len = strlen(name) + 1;
  ast_initlist_entry_explicit_t *out_initlist_entry_explicit =
    malloc(sizeof(ast_initlist_entry_explicit_t) + name_len);
  SOLC_AST_INIT_HEADER(out_initlist_entry_explicit, pos,
                       SOLC_AST_TYPE_INITLIST_ENTRY_EXPLICIT);
  out_initlist_entry_explicit->expr_ast = expr_ast;
  out_initlist_entry_explicit->name =
    (char *)out_initlist_entry_explicit + sizeof(ast_initlist_entry_explicit_t);
  memcpy(out_initlist_entry_explicit->name, name, name_len);
  return SOLC_AST(out_initlist_entry_explicit);
}

void solc_ast_initlist_entry_explicit_destroy(
  solc_ast_t *initlist_entry_explicit_ast)
{
  SOLC_ASSUME(initlist_entry_explicit_ast != nullptr &&
              initlist_entry_explicit_ast->type ==
                SOLC_AST_TYPE_INITLIST_ENTRY_EXPLICIT);
  SOLC_AST_CAST(initlist_entry_explicit_data, initlist_entry_explicit_ast,
                ast_initlist_entry_explicit_t);
  SOLC_ASSUME(initlist_entry_explicit_data->expr_ast != nullptr);
  solc_ast_destroy(initlist_entry_explicit_data->expr_ast);
  free(initlist_entry_explicit_ast);
}

sz solc_ast_initlist_entry_explicit_to_string(
  char *buf, sz n, solc_ast_t *initlist_entry_explicit_ast)
{
  SOLC_ASSUME(buf != nullptr && initlist_entry_explicit_ast != nullptr &&
              initlist_entry_explicit_ast->type ==
                SOLC_AST_TYPE_INITLIST_ENTRY_EXPLICIT);

  SOLC_TODO("Explicit initialization list entry to string.");

  return 0;
}
