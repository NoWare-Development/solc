#include "containers/string.h"
#include "containers/vector.h"
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
  SOLC_ASSUME(name != nullptr);
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
  solc_ast_destroy_if_exists(initlist_entry_explicit_data->expr_ast);
  free(initlist_entry_explicit_ast);
}

string_t *solc_ast_initlist_entry_explicit_build_tree(
  solc_ast_t *initlist_entry_explicit_ast)
{
  SOLC_ASSUME(initlist_entry_explicit_ast != nullptr &&
              initlist_entry_explicit_ast->type ==
                SOLC_AST_TYPE_INITLIST_ENTRY_EXPLICIT);
  SOLC_AST_CAST(initlist_entry_explicit_data, initlist_entry_explicit_ast,
                ast_initlist_entry_explicit_t);
  SOLC_ASSUME(initlist_entry_explicit_data->name != nullptr);

  string_t header = string_create_from("INITLIST_ENTRY_EXPLICIT { name: \"");
  string_append_cstr(&header, initlist_entry_explicit_data->name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v,
                                 initlist_entry_explicit_data->expr_ast);

  return ast_build_tree(&header, children_vs_v);
}
