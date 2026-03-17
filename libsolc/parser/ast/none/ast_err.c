#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"

#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  SOLC_AST_HEADER;
  char *reason;
} ast_err_t;

solc_ast_t *solc_ast_err_create(sz pos, const char *reason)
{
  SOLC_ASSUME(reason != nullptr);

  const sz reason_len = strlen(reason) + 1;

  ast_err_t *out_err = malloc(sizeof(ast_err_t) + reason_len);
  SOLC_AST_INIT_HEADER(out_err, pos, SOLC_AST_TYPE_NONE_ERR);
  out_err->reason = (char *)out_err + sizeof(ast_err_t);
  memcpy(out_err->reason, reason, reason_len);

  return SOLC_AST(out_err);
}

void solc_ast_err_destroy(solc_ast_t *err_ast)
{
  SOLC_ASSUME(err_ast != nullptr && err_ast->type == SOLC_AST_TYPE_NONE_ERR);
  free(err_ast);
}

string_t *solc_ast_err_build_tree(solc_ast_t *err_ast)
{
  SOLC_ASSUME(err_ast != nullptr && err_ast->type == SOLC_AST_TYPE_NONE_ERR);
  SOLC_AST_CAST(err_data, err_ast, ast_err_t);
  SOLC_ASSUME(err_data->reason != nullptr);
  const sz n = strlen(err_data->reason) + 32;
  char *buf = malloc(sizeof(char) * n);
  snprintf(buf, n, "ERR { reason: \"%s\" }", err_data->reason);
  string_t *out_v = vector_reserve(string_t, 1);
  vector_push(out_v, string_create_from(buf));
  free(buf);
  return out_v;
}

const char *solc_ast_err_get_reason(solc_ast_t *err_ast)
{
  SOLC_ASSUME(err_ast != nullptr && err_ast->type == SOLC_AST_TYPE_NONE_ERR);
  SOLC_AST_CAST(err_data, err_ast, ast_err_t);
  SOLC_ASSUME(err_data->reason != nullptr);
  return err_data->reason;
}
