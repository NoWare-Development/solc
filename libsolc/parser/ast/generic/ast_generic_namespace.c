#include "parser/ast/ast_group_generic.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *generic_type_list_ast;
  char *name;
} ast_generic_namespace_t;

solc_ast_t *solc_ast_generic_namespace_create(sz pos, const char *name,
                                              solc_ast_t *generic_type_list_ast)
{
  SOLC_ASSUME(name != nullptr && generic_type_list_ast != nullptr &&
              generic_type_list_ast->type == SOLC_AST_TYPE_GENERIC_TYPE_LIST);
  const sz name_len = strlen(name) + 1;
  ast_generic_namespace_t *out_generic_namespace =
    malloc(sizeof(ast_generic_namespace_t) + name_len);
  SOLC_AST_INIT_HEADER(out_generic_namespace, pos,
                       SOLC_AST_TYPE_GENERIC_NAMESPACE);
  out_generic_namespace->generic_type_list_ast = generic_type_list_ast;
  out_generic_namespace->name =
    (char *)out_generic_namespace + sizeof(ast_generic_namespace_t);
  memcpy(out_generic_namespace->name, name, name_len);
  return SOLC_AST(out_generic_namespace);
}

void solc_ast_generic_namespace_destroy(solc_ast_t *generic_namespace_ast)
{
  SOLC_ASSUME(generic_namespace_ast != nullptr &&
              generic_namespace_ast->type == SOLC_AST_TYPE_GENERIC_NAMESPACE);
  SOLC_AST_CAST(generic_namespace_data, generic_namespace_ast,
                ast_generic_namespace_t);
  SOLC_ASSUME(generic_namespace_data->generic_type_list_ast != nullptr);
  solc_ast_generic_type_list_destroy(
    generic_namespace_data->generic_type_list_ast);
  free(generic_namespace_ast);
}

sz solc_ast_generic_namespace_to_string(char *buf, sz n,
                                        solc_ast_t *generic_namespace_ast)
{
  SOLC_ASSUME(buf != nullptr && generic_namespace_ast != nullptr &&
              generic_namespace_ast->type == SOLC_AST_TYPE_GENERIC_NAMESPACE);

  SOLC_TODO("Generic namespace to string.");

  return 0;
}
