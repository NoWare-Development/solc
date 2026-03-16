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
} ast_vardecl_t;

solc_ast_t *solc_ast_var_decl_create(sz pos, const char *name,
                                     solc_ast_t *type_ast)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_vardecl_t *out_vardecl = malloc(sizeof(ast_vardecl_t) + name_len);
  SOLC_AST_INIT_HEADER(out_vardecl, pos, SOLC_AST_TYPE_VAR_DECL);
  out_vardecl->type_ast = type_ast;
  out_vardecl->name = (char *)out_vardecl + sizeof(ast_vardecl_t);
  memcpy(out_vardecl->name, name, name_len);
  return SOLC_AST(out_vardecl);
}

void solc_ast_var_decl_destroy(solc_ast_t *var_decl_ast)
{
  SOLC_ASSUME(var_decl_ast != nullptr &&
              var_decl_ast->type == SOLC_AST_TYPE_VAR_DECL);
  SOLC_AST_CAST(vardecl_data, var_decl_ast, ast_vardecl_t);
  solc_ast_destroy_if_exists(vardecl_data->type_ast);
  free(var_decl_ast);
}

string_t *solc_ast_var_decl_build_tree(solc_ast_t *var_decl_ast)
{
  SOLC_ASSUME(var_decl_ast != nullptr &&
              var_decl_ast->type == SOLC_AST_TYPE_VAR_DECL);
  SOLC_AST_CAST(vardecl_data, var_decl_ast, ast_vardecl_t);
  SOLC_ASSUME(vardecl_data->name != nullptr);
  string_t header = string_create_from("VAR_DECL { name: \"");
  string_append_cstr(&header, vardecl_data->name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v, vardecl_data->type_ast);

  return ast_build_tree(&header, children_vs_v);
}

const char *solc_ast_var_decl_get_name(solc_ast_t *var_decl_ast)
{
  SOLC_ASSUME(var_decl_ast != nullptr &&
              var_decl_ast->type == SOLC_AST_TYPE_VAR_DECL);
  SOLC_AST_CAST(vardecl_data, var_decl_ast, ast_vardecl_t);
  SOLC_ASSUME(vardecl_data->name != nullptr);
  return vardecl_data->name;
}

solc_ast_t *solc_ast_var_decl_get_type_ast(solc_ast_t *var_decl_ast)
{
  SOLC_ASSUME(var_decl_ast != nullptr &&
              var_decl_ast->type == SOLC_AST_TYPE_VAR_DECL);
  SOLC_AST_CAST(vardecl_data, var_decl_ast, ast_vardecl_t);
  return vardecl_data->type_ast;
}
