#include "containers/string.h"
#include "containers/vector.h"
#include "solc/parser/ast.h"
#include "parser/ast_private.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *name;
  solc_ast_t *type_ast;
} ast_extern_vardecl_t;

solc_ast_t *solc_ast_extern_vardecl_create(sz pos, const char *name,
                                           solc_ast_t *type_ast)
{
  SOLC_ASSUME(name != nullptr);
  const sz name_len = strlen(name) + 1;
  ast_extern_vardecl_t *out_extern_vardecl =
    malloc(sizeof(ast_extern_vardecl_t) + name_len);
  SOLC_AST_INIT_HEADER(out_extern_vardecl, pos,
                       SOLC_AST_TYPE_NONE_EXTERN_VARDECL);
  out_extern_vardecl->name =
    (char *)out_extern_vardecl + sizeof(ast_extern_vardecl_t);
  memcpy(out_extern_vardecl->name, name, name_len);
  out_extern_vardecl->type_ast = type_ast;
  return SOLC_AST(out_extern_vardecl);
}

void solc_ast_extern_vardecl_destroy(solc_ast_t *extern_vardecl_ast)
{
  SOLC_ASSUME(extern_vardecl_ast != nullptr &&
              extern_vardecl_ast->type == SOLC_AST_TYPE_NONE_EXTERN_VARDECL);
  SOLC_AST_CAST(extern_vardecl_data, extern_vardecl_ast, ast_extern_vardecl_t);
  solc_ast_destroy_if_exists(extern_vardecl_data->type_ast);
  free(extern_vardecl_data);
}

string_t *solc_ast_extern_vardecl_build_tree(solc_ast_t *extern_vardecl_ast)
{
  SOLC_ASSUME(extern_vardecl_ast != nullptr &&
              extern_vardecl_ast->type == SOLC_AST_TYPE_NONE_EXTERN_VARDECL);
  SOLC_AST_CAST(extern_vardecl_data, extern_vardecl_ast, ast_extern_vardecl_t);
  SOLC_ASSUME(extern_vardecl_data->name != nullptr);

  string_t header = string_create_from("EXTERN_VARDECL { name: \"");
  string_append_cstr(&header, extern_vardecl_data->name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v, extern_vardecl_data->type_ast);

  return ast_build_tree(&header, children_vs_v);
}
