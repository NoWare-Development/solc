#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **children_v;
  char *name;
} ast_union_t;

solc_ast_t *solc_ast_union_create(sz pos, const char *name)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_union_t *out_union = malloc(sizeof(ast_union_t) + name_len);
  SOLC_AST_INIT_HEADER(out_union, pos, SOLC_AST_TYPE_NONE_UNION);
  out_union->children_v = vector_create(solc_ast_t *);
  out_union->name = (char *)out_union + sizeof(ast_union_t);
  memcpy(out_union->name, name, name_len);
  return SOLC_AST(out_union);
}

void solc_ast_union_destroy(solc_ast_t *union_ast)
{
  SOLC_ASSUME(union_ast != nullptr &&
              union_ast->type == SOLC_AST_TYPE_NONE_UNION);
  SOLC_AST_CAST(union_data, union_ast, ast_union_t);
  SOLC_ASSUME(union_data->children_v != nullptr);
  for (sz i = 0, children_v_size = vector_get_length(union_data->children_v);
       i < children_v_size; i++)
    solc_ast_destroy_if_exists(union_data->children_v[i]);
  vector_destroy(union_data->children_v);
  free(union_data);
}

void solc_ast_union_add_child(solc_ast_t *union_ast, solc_ast_t *child_ast)
{
  SOLC_ASSUME(union_ast != nullptr &&
              union_ast->type == SOLC_AST_TYPE_NONE_UNION);
  SOLC_AST_CAST(union_data, union_ast, ast_union_t);
  SOLC_ASSUME(union_data->children_v != nullptr);
  vector_push(union_data->children_v, child_ast);
}

string_t *solc_ast_union_build_tree(solc_ast_t *union_ast)
{
  SOLC_ASSUME(union_ast != nullptr &&
              union_ast->type == SOLC_AST_TYPE_NONE_UNION);
  SOLC_AST_CAST(union_data, union_ast, ast_union_t);
  SOLC_ASSUME(union_data->children_v != nullptr && union_data->name != nullptr);

  string_t header = string_create_from("UNION { name: \"");
  string_append_cstr(&header, union_data->name);
  string_append_cstr(&header, "\" }");

  sz children_v_size = vector_get_length(union_data->children_v);
  string_t **children_vs_v = vector_reserve(string_t *, children_v_size);
  for (sz i = 0; i < children_v_size; i++)
    solc_ast_add_to_tree_if_exists(children_vs_v, union_data->children_v[i]);

  return ast_build_tree(&header, children_vs_v);
}
