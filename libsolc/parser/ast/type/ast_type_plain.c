#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *name;
} ast_plain_type_t;

solc_ast_t *solc_ast_type_plain_create(sz pos, const char *name)
{
  SOLC_ASSUME(name != nullptr);
  const sz name_len = strlen(name) + 1;
  ast_plain_type_t *out_plain_type =
    malloc(sizeof(ast_plain_type_t) + name_len);
  SOLC_AST_INIT_HEADER(out_plain_type, pos, SOLC_AST_TYPE_TYPE_PLAIN);
  out_plain_type->name = (char *)out_plain_type + sizeof(ast_plain_type_t);
  memcpy(out_plain_type->name, name, name_len);
  return SOLC_AST(out_plain_type);
}

void solc_ast_type_plain_destroy(solc_ast_t *plain_type_ast)
{
  SOLC_ASSUME(plain_type_ast != nullptr &&
              plain_type_ast->type == SOLC_AST_TYPE_TYPE_PLAIN);
  free(plain_type_ast);
}

sz solc_ast_type_plain_to_string(char *buf, sz n, solc_ast_t *plain_type_ast)
{
  SOLC_ASSUME(buf != nullptr && plain_type_ast != nullptr);
  SOLC_AST_CAST(plain_type_data, plain_type_ast, ast_plain_type_t);
  SOLC_ASSUME(plain_type_data->name != nullptr);
  return snprintf(buf, n, "TYPE_PLAIN { name: \"%s\" }", plain_type_data->name);
}
