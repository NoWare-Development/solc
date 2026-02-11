#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *name;
} ast_label_stmt_t;

solc_ast_t *solc_ast_stmt_label_create(sz pos, const char *name)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_label_stmt_t *out_label_stmt =
    malloc(sizeof(ast_label_stmt_t) + name_len);
  SOLC_AST_INIT_HEADER(out_label_stmt, pos, SOLC_AST_TYPE_STMT_LABEL);
  out_label_stmt->name = (char *)out_label_stmt + sizeof(ast_label_stmt_t);
  memcpy(out_label_stmt->name, name, name_len);
  return SOLC_AST(out_label_stmt);
}

void solc_ast_stmt_label_destroy(solc_ast_t *label_ast)
{
  SOLC_ASSUME(label_ast != nullptr &&
              label_ast->type == SOLC_AST_TYPE_STMT_LABEL);

  free(label_ast);
}

sz solc_ast_stmt_label_to_string(char *buf, sz n, solc_ast_t *label_ast)
{
  SOLC_ASSUME(buf != nullptr && label_ast != nullptr &&
              label_ast->type == SOLC_AST_TYPE_STMT_LABEL);
  SOLC_AST_CAST(label_data, label_ast, ast_label_stmt_t);
  SOLC_ASSUME(label_data->name != nullptr);
  return snprintf(buf, n, "STMT_LABEL { name: \"%s\" }", label_data->name);
}
