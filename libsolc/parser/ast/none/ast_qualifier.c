#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *qualified_ast;
  char *name;
} ast_qualifier_t;

solc_ast_t *solc_ast_qualifier_create(sz pos, const char *name,
                                      solc_ast_t *qualified_ast)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_qualifier_t *out_qualifier = malloc(sizeof(ast_qualifier_t) + name_len);
  SOLC_AST_INIT_HEADER(out_qualifier, pos, SOLC_AST_TYPE_NONE_QUALIFIER);
  out_qualifier->qualified_ast = qualified_ast;
  out_qualifier->name = (char *)out_qualifier + sizeof(ast_qualifier_t);
  memcpy(out_qualifier->name, name, name_len);
  return SOLC_AST(out_qualifier);
}

void solc_ast_qualifier_destroy(solc_ast_t *qualifier_ast)
{
  SOLC_ASSUME(qualifier_ast != nullptr &&
              qualifier_ast->type == SOLC_AST_TYPE_NONE_QUALIFIER);
  SOLC_AST_CAST(qualifier_data, qualifier_ast, ast_qualifier_t);
  solc_ast_destroy_if_exists(qualifier_data->qualified_ast);
  free(qualifier_ast);
}

string_t *solc_ast_qualifier_build_tree(solc_ast_t *qualifier_ast)
{
  SOLC_ASSUME(qualifier_ast != nullptr &&
              qualifier_ast->type == SOLC_AST_TYPE_NONE_QUALIFIER);
  SOLC_AST_CAST(qualifier_data, qualifier_ast, ast_qualifier_t);
  SOLC_ASSUME(qualifier_data->name != nullptr);

  string_t header = string_create_from("QUALIFIER { name: \"");
  string_append_cstr(&header, qualifier_data->name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v, qualifier_data->qualified_ast);

  return ast_build_tree(&header, children_vs_v);
}
