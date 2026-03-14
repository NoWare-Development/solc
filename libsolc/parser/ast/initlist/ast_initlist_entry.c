#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
} ast_initlist_entry_t;

solc_ast_t *solc_ast_initlist_entry_create(sz pos, solc_ast_t *expr_ast)
{
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
  solc_ast_destroy_if_exists(initlist_entry_data->expr_ast);
  free(initlist_entry_ast);
}

string_t *solc_ast_initlist_entry_build_tree(solc_ast_t *initlist_entry_ast)
{
  SOLC_ASSUME(initlist_entry_ast != nullptr &&
              initlist_entry_ast->type == SOLC_AST_TYPE_INITLIST_ENTRY);
  SOLC_AST_CAST(initlist_entry_data, initlist_entry_ast, ast_initlist_entry_t);
  string_t header = string_create_from("INITLIST_ENTRY");
  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v, initlist_entry_data->expr_ast);

  return ast_build_tree(&header, children_vs_v);
}
