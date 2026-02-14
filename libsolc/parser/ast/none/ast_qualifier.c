#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *name;
} ast_qualifier_t;

solc_ast_t *solc_ast_qualifier_create(sz pos, const char *name)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_qualifier_t *out_qualifier = malloc(sizeof(ast_qualifier_t) + name_len);
  SOLC_AST_INIT_HEADER(out_qualifier, pos, SOLC_AST_TYPE_NONE_QUALIFIER);
  out_qualifier->name = (char *)out_qualifier + sizeof(ast_qualifier_t);
  memcpy(out_qualifier->name, name, name_len);
  return SOLC_AST(out_qualifier);
}

void solc_ast_qualifier_destroy(solc_ast_t *qualifier_ast)
{
  SOLC_ASSUME(qualifier_ast != nullptr &&
              qualifier_ast->type == SOLC_AST_TYPE_NONE_QUALIFIER);
  free(qualifier_ast);
}

string_t *solc_ast_qualifier_build_tree(solc_ast_t *qualifier_ast)
{
  SOLC_ASSUME(qualifier_ast != nullptr &&
              qualifier_ast->type == SOLC_AST_TYPE_NONE_QUALIFIER);
  SOLC_AST_CAST(qualifier_data, qualifier_ast, ast_qualifier_t);
  SOLC_ASSUME(qualifier_data->name != nullptr);
  const sz n = 32 + strlen(qualifier_data->name);
  char *buf = malloc(sizeof(char) * n);
  snprintf(buf, n, "QUALIFIER { name: \"%s\" }", qualifier_data->name);
  string_t *out_v = vector_reserve(string_t, 1);
  vector_push(out_v, string_create_from(buf));
  free(buf);
  return out_v;
}
