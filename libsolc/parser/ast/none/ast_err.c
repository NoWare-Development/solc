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

sz solc_ast_err_to_string(char *buf, sz n, solc_ast_t *err_ast)
{
  SOLC_ASSUME(buf != nullptr && err_ast != nullptr &&
              err_ast->type == SOLC_AST_TYPE_NONE_ERR);

  SOLC_AST_CAST(err_data, err_ast, ast_err_t);
  return snprintf(buf, n, "ERR { reason: \"%s\" }", err_data->reason);
}
