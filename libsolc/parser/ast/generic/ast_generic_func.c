#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_func_type.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *name;
  solc_ast_t *type_ast;
  solc_ast_t *arg_list_ast;
  solc_ast_t *block_ast;
  solc_ast_t *generic_placeholder_type_list_ast;
  solc_ast_func_type_t func_type;
} ast_generic_func_t;

solc_ast_t *
solc_ast_generic_func_create(sz pos, const char *name, solc_ast_t *type_ast,
                             solc_ast_t *arg_list_ast, solc_ast_t *block_ast,
                             solc_ast_t *generic_placeholder_type_list_ast,
                             solc_ast_func_type_t func_type)
{
  SOLC_ASSUME(name != nullptr);
  const sz name_len = strlen(name) + 1;
  ast_generic_func_t *out_generic_func =
    malloc(sizeof(ast_generic_func_t) + name_len);
  SOLC_AST_INIT_HEADER(out_generic_func, pos, SOLC_AST_TYPE_GENERIC_FUNC);
  out_generic_func->name =
    (char *)out_generic_func + sizeof(ast_generic_func_t);
  memcpy(out_generic_func->name, name, name_len);
  out_generic_func->type_ast = type_ast;
  out_generic_func->arg_list_ast = arg_list_ast;
  out_generic_func->block_ast = block_ast;
  out_generic_func->generic_placeholder_type_list_ast =
    generic_placeholder_type_list_ast;
  out_generic_func->func_type = func_type;
  return SOLC_AST(out_generic_func);
}

void solc_ast_generic_func_destroy(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  solc_ast_destroy_if_exists(generic_func_data->type_ast);
  solc_ast_destroy_if_exists(generic_func_data->arg_list_ast);
  solc_ast_destroy_if_exists(generic_func_data->block_ast);
  solc_ast_destroy_if_exists(
    generic_func_data->generic_placeholder_type_list_ast);
  free(generic_func_data);
}

string_t *solc_ast_generic_func_build_tree(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  SOLC_ASSUME(generic_func_data->name != nullptr);

  char header_cstr[1024] = { 0 };
  snprintf(header_cstr, 1023, "GENERIC_FUNC { name: \"%s\", type: %s }",
           generic_func_data->name,
           solc_ast_func_type_to_string(generic_func_data->func_type));
  string_t header = string_create_from(header_cstr);

  string_t **children_vs_v = vector_reserve(string_t *, 4);
  solc_ast_add_to_tree_if_exists(children_vs_v, generic_func_data->type_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v,
                                 generic_func_data->arg_list_ast);
  solc_ast_add_to_tree_if_exists(
    children_vs_v, generic_func_data->generic_placeholder_type_list_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, generic_func_data->block_ast);
  return ast_build_tree(&header, children_vs_v);
}

const char *solc_ast_generic_func_get_name(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  SOLC_ASSUME(generic_func_data->name != nullptr);
  return generic_func_data->name;
}

solc_ast_t *solc_ast_generic_func_get_type_ast(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  return generic_func_data->type_ast;
}

solc_ast_t *solc_ast_generic_func_get_arg_list_ast(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  return generic_func_data->arg_list_ast;
}

solc_ast_t *solc_ast_generic_func_get_block_ast(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  return generic_func_data->block_ast;
}

solc_ast_t *solc_ast_generic_func_get_generic_placeholder_type_list_ast(
  solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  return generic_func_data->generic_placeholder_type_list_ast;
}

solc_ast_func_type_t
solc_ast_generic_func_get_func_type(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  return generic_func_data->func_type;
}

b8 solc_ast_generic_func_is_explicit(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  return generic_func_data->func_type == SOLC_AST_FUNC_TYPE_EXPLICIT;
}

b8 solc_ast_generic_func_is_exported(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  return generic_func_data->func_type == SOLC_AST_FUNC_TYPE_EXPORTED;
}
