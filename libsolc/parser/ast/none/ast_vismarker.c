#include "containers/string.h"
#include "containers/vector.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include "parser/ast_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *name;
} ast_vismarker_t;

solc_ast_t *solc_ast_vismarker_create(sz pos, const char *name)
{
  SOLC_ASSUME(name != nullptr);
  const sz name_len = strlen(name) + 1;
  ast_vismarker_t *out_vismarker = malloc(sizeof(ast_vismarker_t) + name_len);
  SOLC_AST_INIT_HEADER(out_vismarker, pos, SOLC_AST_TYPE_NONE_VISMARKER);
  out_vismarker->name = (char *)out_vismarker + sizeof(ast_vismarker_t);
  memcpy(out_vismarker->name, name, name_len);
  return SOLC_AST(out_vismarker);
}

void solc_ast_vismarker_destroy(solc_ast_t *vismarker_ast)
{
  SOLC_ASSUME(vismarker_ast != nullptr &&
              vismarker_ast->type == SOLC_AST_TYPE_NONE_VISMARKER);
  free(vismarker_ast);
}

string_t *solc_ast_vismarker_build_tree(solc_ast_t *vismarker_ast)
{
  SOLC_ASSUME(vismarker_ast != nullptr &&
              vismarker_ast->type == SOLC_AST_TYPE_NONE_VISMARKER);
  SOLC_AST_CAST(vismarker_data, vismarker_ast, ast_vismarker_t);
  SOLC_ASSUME(vismarker_data->name != nullptr);

  const sz n = sizeof("VISMARKER { name: \"") + sizeof("\" }") +
               strlen(vismarker_data->name);
  char *buf = malloc(sizeof(char) * (n + 1));
  snprintf(buf, n, "VISMARKER { name: \"%s\" }", vismarker_data->name);

  string_t *out = vector_reserve(string_t, 1);
  vector_push(out, string_create_from(buf));

  free(buf);
  return out;
}

const char *solc_ast_vismarker_get_name(solc_ast_t *vismarker_ast)
{
  SOLC_ASSUME(vismarker_ast != nullptr &&
              vismarker_ast->type == SOLC_AST_TYPE_NONE_VISMARKER);
  SOLC_AST_CAST(vismarker_data, vismarker_ast, ast_vismarker_t);
  SOLC_ASSUME(vismarker_data->name != nullptr);

  return vismarker_data->name;
}
