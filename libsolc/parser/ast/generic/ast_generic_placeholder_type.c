#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *name;
} ast_generic_placeholder_type_t;

solc_ast_t *solc_ast_generic_placeholder_type_create(sz pos, const char *name)
{
  SOLC_ASSUME(name != nullptr);
  const sz name_len = strlen(name) + 1;
  ast_generic_placeholder_type_t *out_generic_placeholder =
    malloc(sizeof(ast_generic_placeholder_type_t) + name_len);
  SOLC_AST_INIT_HEADER(out_generic_placeholder, pos,
                       SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE);
  out_generic_placeholder->name =
    (char *)out_generic_placeholder + sizeof(ast_generic_placeholder_type_t);
  memcpy(out_generic_placeholder->name, name, name_len);
  return SOLC_AST(out_generic_placeholder);
}

void solc_ast_generic_placeholder_type_destroy(
  solc_ast_t *generic_placeholder_type_ast)
{
  SOLC_ASSUME(generic_placeholder_type_ast != nullptr &&
              generic_placeholder_type_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE);
  free(generic_placeholder_type_ast);
}

sz solc_ast_generic_placeholder_type_to_string(
  char *buf, sz n, solc_ast_t *generic_placeholder_type_ast)
{
  SOLC_ASSUME(buf != nullptr && generic_placeholder_type_ast != nullptr &&
              generic_placeholder_type_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE);

  SOLC_AST_CAST(generic_placeholder_type_data, generic_placeholder_type_ast,
                ast_generic_placeholder_type_t);
  SOLC_ASSUME(generic_placeholder_type_data->name != nullptr);
  return snprintf(buf, n, "GENERIC_PLACEHOLDER_TYPE { name: \"%s\" }",
                  generic_placeholder_type_data->name);
}
