#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *subobject;
  char *name;
} ast_namespace_t;

solc_ast_t *solc_ast_namespace_create(sz pos, const char *name,
                                      solc_ast_t *subobject)
{
  SOLC_ASSUME(name != nullptr && subobject != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_namespace_t *out_namespace = malloc(sizeof(ast_namespace_t) + name_len);
  SOLC_AST_INIT_HEADER(out_namespace, pos, SOLC_AST_TYPE_NONE_NAMESPACE);
  out_namespace->subobject = subobject;
  out_namespace->name = (char *)out_namespace + sizeof(ast_namespace_t);
  memcpy(out_namespace->name, name, name_len);
  return SOLC_AST(out_namespace);
}

void solc_ast_namespace_destroy(solc_ast_t *namespace_ast)
{
  SOLC_ASSUME(namespace_ast != nullptr &&
              namespace_ast->type == SOLC_AST_TYPE_NONE_NAMESPACE);

  SOLC_AST_CAST(namespace_data, namespace_ast, ast_namespace_t);
  SOLC_ASSUME(namespace_data->subobject != nullptr);
  solc_ast_destroy(namespace_data->subobject);
  free(namespace_data);
}

sz solc_ast_namespace_to_string(char *buf, sz n, solc_ast_t *namespace_ast)
{
  SOLC_ASSUME(buf != nullptr && namespace_ast != nullptr &&
              namespace_ast->type == SOLC_AST_TYPE_NONE_NAMESPACE);

  SOLC_TODO("Namespace to string.");

  return 0;
}
