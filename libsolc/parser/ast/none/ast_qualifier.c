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

sz solc_ast_qualifier_to_string(char *buf, sz n, solc_ast_t *qualifier_ast)
{
  SOLC_ASSUME(qualifier_ast != nullptr &&
              qualifier_ast->type == SOLC_AST_TYPE_NONE_QUALIFIER);
  SOLC_AST_CAST(qualifier_data, qualifier_ast, ast_qualifier_t);

  return snprintf(buf, n, "QUALIFIER { name: \"%s\" }", qualifier_data->name);
}
