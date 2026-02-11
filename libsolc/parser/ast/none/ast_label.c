#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *name;
} ast_label_t;

solc_ast_t *solc_ast_label_create(sz pos, const char *name)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_label_t *out_label = malloc(sizeof(ast_label_t) + name_len);
  SOLC_AST_INIT_HEADER(out_label, pos, SOLC_AST_TYPE_NONE_LABEL);
  out_label->name = (char *)out_label + sizeof(ast_label_t);
  memcpy(out_label->name, name, name_len);
  return SOLC_AST(out_label);
}

void solc_ast_label_destroy(solc_ast_t *label_ast)
{
  SOLC_ASSUME(label_ast != nullptr &&
              label_ast->type == SOLC_AST_TYPE_NONE_LABEL);
  free(label_ast);
}

sz solc_ast_label_to_string(char *buf, sz n, solc_ast_t *label_ast)
{
  SOLC_ASSUME(buf != nullptr && label_ast != nullptr &&
              label_ast->type == SOLC_AST_TYPE_NONE_LABEL);

  SOLC_AST_CAST(label_data, label_ast, ast_label_t);
  return snprintf(buf, n, "LABEL { name: \"%s\" }", label_data->name);
}
