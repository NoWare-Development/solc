#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
  char *name;
} ast_typedef_t;

solc_ast_t *solc_ast_typedef_create(sz pos, solc_ast_t *type_ast,
                                    const char *name)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_typedef_t *out_typedef = malloc(sizeof(ast_typedef_t) + name_len);
  SOLC_AST_INIT_HEADER(out_typedef, pos, SOLC_AST_TYPE_NONE_TYPEDEF);
  out_typedef->type_ast = type_ast;
  out_typedef->name = (char *)out_typedef + sizeof(ast_typedef_t);
  memcpy(out_typedef->name, name, name_len);
  return SOLC_AST(out_typedef);
}

void solc_ast_typedef_destroy(solc_ast_t *typedef_ast)
{
  SOLC_ASSUME(typedef_ast != nullptr &&
              typedef_ast->type == SOLC_AST_TYPE_NONE_TYPEDEF);
  SOLC_AST_CAST(typedef_data, typedef_ast, ast_typedef_t);
  solc_ast_destroy_if_exists(typedef_data->type_ast);
  free(typedef_data);
}

string_t *solc_ast_typedef_build_tree(solc_ast_t *typedef_ast)
{
  SOLC_ASSUME(typedef_ast != nullptr &&
              typedef_ast->type == SOLC_AST_TYPE_NONE_TYPEDEF);
  SOLC_AST_CAST(typedef_data, typedef_ast, ast_typedef_t);
  SOLC_ASSUME(typedef_data->name != nullptr);
  string_t header = string_create_from("TYPEDEF { name: \"");
  string_append_cstr(&header, typedef_data->name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v, typedef_data->type_ast);
  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t *solc_ast_typedef_get_type_ast(solc_ast_t *typedef_ast)
{
  SOLC_ASSUME(typedef_ast != nullptr &&
              typedef_ast->type == SOLC_AST_TYPE_NONE_TYPEDEF);
  SOLC_AST_CAST(typedef_data, typedef_ast, ast_typedef_t);
  return typedef_data->type_ast;
}

const char *solc_ast_typedef_get_name(solc_ast_t *typedef_ast)
{
  SOLC_ASSUME(typedef_ast != nullptr &&
              typedef_ast->type == SOLC_AST_TYPE_NONE_TYPEDEF);
  SOLC_AST_CAST(typedef_data, typedef_ast, ast_typedef_t);
  SOLC_ASSUME(typedef_data->name != nullptr);
  return typedef_data->name;
}
