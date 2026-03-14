#include "containers/vector.h"
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

string_t *solc_ast_type_plain_build_tree(solc_ast_t *plain_type_ast)
{
  SOLC_ASSUME(plain_type_ast != nullptr &&
              plain_type_ast->type == SOLC_AST_TYPE_TYPE_PLAIN);
  SOLC_AST_CAST(plain_type_data, plain_type_ast, ast_plain_type_t);
  SOLC_ASSUME(plain_type_data->name != nullptr);

  const sz name_len = strlen(plain_type_data->name);
  char *buf = malloc(sizeof(char) * (128 + name_len));
  snprintf(buf, 128 + name_len, "TYPE_PLAIN { name: \"%s\" }",
           plain_type_data->name);

  string_t *out_v = vector_reserve(string_t, 1);
  vector_push(out_v, string_create_from(buf));
  free(buf);

  return out_v;
}
