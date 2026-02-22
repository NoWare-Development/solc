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
  solc_ast_t *arg_list_ast;
  solc_ast_t *block_ast;
  char *name;
} ast_func_t;

solc_ast_t *solc_ast_func_create(sz pos, const char *name, solc_ast_t *type_ast,
                                 solc_ast_t *arg_list_ast,
                                 solc_ast_t *block_ast)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_func_t *out_func = malloc(sizeof(ast_func_t) + name_len);
  SOLC_AST_INIT_HEADER(out_func, pos, SOLC_AST_TYPE_NONE_FUNC);
  out_func->type_ast = type_ast;
  out_func->arg_list_ast = arg_list_ast;
  out_func->block_ast = block_ast;
  out_func->name = (char *)out_func + sizeof(ast_func_t);
  memcpy(out_func->name, name, name_len);
  return SOLC_AST(out_func);
}

void solc_ast_func_destroy(solc_ast_t *func_ast)
{
  SOLC_ASSUME(func_ast != nullptr && func_ast->type == SOLC_AST_TYPE_NONE_FUNC);
  SOLC_AST_CAST(func_data, func_ast, ast_func_t);

  solc_ast_destroy_if_exists(func_data->type_ast);
  solc_ast_destroy_if_exists(func_data->arg_list_ast);
  solc_ast_destroy_if_exists(func_data->block_ast);

  free(func_ast);
}

string_t *solc_ast_func_build_tree(solc_ast_t *func_ast)
{
  SOLC_ASSUME(func_ast != nullptr && func_ast->type == SOLC_AST_TYPE_NONE_FUNC);
  SOLC_AST_CAST(func_data, func_ast, ast_func_t);
  SOLC_ASSUME(func_data->name != nullptr);
  string_t header = string_create_from("FUNC { name: \"");
  string_append_cstr(&header, func_data->name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 3);
  solc_ast_add_to_tree_if_exists(children_vs_v, func_data->type_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, func_data->arg_list_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, func_data->block_ast);

  return ast_build_tree(&header, children_vs_v);
}

const char *solc_ast_func_get_name(solc_ast_t *func_ast)
{
  SOLC_ASSUME(func_ast != nullptr &&
              (func_ast->type == SOLC_AST_TYPE_NONE_FUNC ||
               func_ast->type == SOLC_AST_TYPE_NONE_FUNC_EXPLICIT ||
               func_ast->type == SOLC_AST_TYPE_NONE_EXPORTED_FUNC));
  SOLC_AST_CAST(func_data, func_ast, ast_func_t);
  SOLC_ASSUME(func_data->name != nullptr);
  return func_data->name;
}

solc_ast_t *solc_ast_func_get_type_ast(solc_ast_t *func_ast)
{
  SOLC_ASSUME(func_ast != nullptr &&
              (func_ast->type == SOLC_AST_TYPE_NONE_FUNC ||
               func_ast->type == SOLC_AST_TYPE_NONE_FUNC_EXPLICIT ||
               func_ast->type == SOLC_AST_TYPE_NONE_EXPORTED_FUNC));
  SOLC_AST_CAST(func_data, func_ast, ast_func_t);
  return func_data->type_ast;
}

solc_ast_t *solc_ast_func_get_arg_list_ast(solc_ast_t *func_ast)
{
  SOLC_ASSUME(func_ast != nullptr &&
              (func_ast->type == SOLC_AST_TYPE_NONE_FUNC ||
               func_ast->type == SOLC_AST_TYPE_NONE_FUNC_EXPLICIT ||
               func_ast->type == SOLC_AST_TYPE_NONE_EXPORTED_FUNC));
  SOLC_AST_CAST(func_data, func_ast, ast_func_t);
  return func_data->arg_list_ast;
}

solc_ast_t *solc_ast_func_get_block_ast(solc_ast_t *func_ast)
{
  SOLC_ASSUME(func_ast != nullptr &&
              (func_ast->type == SOLC_AST_TYPE_NONE_FUNC ||
               func_ast->type == SOLC_AST_TYPE_NONE_FUNC_EXPLICIT ||
               func_ast->type == SOLC_AST_TYPE_NONE_EXPORTED_FUNC));
  SOLC_AST_CAST(func_data, func_ast, ast_func_t);
  return func_data->block_ast;
}
