#include "containers/string.h"
#include "containers/vector.h"
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
  SOLC_ASSUME(name != nullptr);
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
  solc_ast_destroy_if_exists(generic_type_data->generic_type_list_ast);
  free(generic_type_ast);
}

string_t *solc_ast_generic_type_build_tree(solc_ast_t *generic_type_ast)
{
  SOLC_ASSUME(generic_type_ast != nullptr &&
              generic_type_ast->type == SOLC_AST_TYPE_GENERIC_TYPE);
  SOLC_AST_CAST(generic_type_data, generic_type_ast, ast_generic_type_t);
  SOLC_ASSUME(generic_type_data->name != nullptr);

  string_t header = string_create_from("GENERIC_TYPE { name: \"");
  string_append_cstr(&header, generic_type_data->name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v,
                                 generic_type_data->generic_type_list_ast);

  return ast_build_tree(&header, children_vs_v);
}
