#include "parser/ast/ast_group_generic.h"
#include "parser/ast_private.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *generic_type_list_ast;
  char *name;
} ast_generic_type_t;

solc_ast_t *solc_ast_generic_type_create(sz pos, const char *name,
                                         solc_ast_t *generic_type_list_ast)
{
  SOLC_ASSUME(name != nullptr && generic_type_list_ast != nullptr &&
              generic_type_list_ast->type == SOLC_AST_TYPE_GENERIC_TYPE_LIST);
  const sz name_len = strlen(name) + 1;
  ast_generic_type_t *out_generic_type =
    malloc(sizeof(ast_generic_type_t) + name_len);
  SOLC_AST_INIT_HEADER(out_generic_type, pos, SOLC_AST_TYPE_GENERIC_TYPE);
  out_generic_type->generic_type_list_ast = generic_type_list_ast;
  out_generic_type->name =
    (char *)out_generic_type + sizeof(ast_generic_type_t);
  memcpy(out_generic_type->name, name, name_len);
  return SOLC_AST(out_generic_type);
}

void solc_ast_generic_type_destroy(solc_ast_t *generic_type_ast)
{
  SOLC_ASSUME(generic_type_ast != nullptr &&
              generic_type_ast->type == SOLC_AST_TYPE_GENERIC_TYPE);
  SOLC_AST_CAST(generic_type_data, generic_type_ast, ast_generic_type_t);
  SOLC_ASSUME(generic_type_data->generic_type_list_ast != nullptr &&
              generic_type_data->generic_type_list_ast->type ==
                SOLC_AST_TYPE_GENERIC_TYPE_LIST);
  solc_ast_generic_type_list_destroy(generic_type_data->generic_type_list_ast);
  free(generic_type_ast);
}

sz solc_ast_generic_type_to_string(char *buf, sz n,
                                   solc_ast_t *generic_type_ast)
{
  SOLC_ASSUME(buf != nullptr && generic_type_ast != nullptr &&
              generic_type_ast->type == SOLC_AST_TYPE_GENERIC_TYPE);

  SOLC_TODO("Generic type to string.");

  return 0;
}
