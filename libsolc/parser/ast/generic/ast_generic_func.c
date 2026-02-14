#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
  solc_ast_t *arg_list_ast;
  solc_ast_t *block_ast;
  solc_ast_t *generic_placeholder_type_list_ast;
  char *name;
} ast_generic_func_t;

solc_ast_t *
solc_ast_generic_func_create(sz pos, const char *name, solc_ast_t *type_ast,
                             solc_ast_t *arg_list_ast, solc_ast_t *block_ast,
                             solc_ast_t *generic_placeholder_type_list_ast)
{
  SOLC_ASSUME(name != nullptr && arg_list_ast != nullptr &&
              arg_list_ast->type == SOLC_AST_TYPE_NONE_FUNC_ARGLIST &&
              block_ast != nullptr &&
              block_ast->type == SOLC_AST_TYPE_STMT_BLOCK &&
              generic_placeholder_type_list_ast != nullptr &&
              generic_placeholder_type_list_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE_LIST);
  const sz name_len = strlen(name) + 1;
  ast_generic_func_t *out_generic_func =
    malloc(sizeof(ast_generic_func_t) + name_len);
  SOLC_AST_INIT_HEADER(out_generic_func, pos, SOLC_AST_TYPE_GENERIC_FUNC);
  out_generic_func->type_ast = type_ast;
  out_generic_func->arg_list_ast = arg_list_ast;
  out_generic_func->block_ast = block_ast;
  out_generic_func->generic_placeholder_type_list_ast =
    generic_placeholder_type_list_ast;
  out_generic_func->name =
    (char *)out_generic_func + sizeof(ast_generic_func_t);
  memcpy(out_generic_func->name, name, name_len);
  return SOLC_AST(out_generic_func);
}

void solc_ast_generic_func_destroy(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              (generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC ||
               generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT));
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  SOLC_ASSUME(generic_func_data->arg_list_ast != nullptr &&
              generic_func_data->block_ast != nullptr &&
              generic_func_data->generic_placeholder_type_list_ast != nullptr);
  if (generic_func_data->type_ast != nullptr) {
    solc_ast_destroy(generic_func_data->type_ast);
  }
  solc_ast_destroy(generic_func_data->arg_list_ast);
  solc_ast_destroy(generic_func_data->block_ast);
  solc_ast_destroy(generic_func_data->generic_placeholder_type_list_ast);
  free(generic_func_data);
}

string_t *solc_ast_generic_func_build_tree(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  SOLC_ASSUME(generic_func_data->arg_list_ast != nullptr &&
              generic_func_data->arg_list_ast->type ==
                SOLC_AST_TYPE_NONE_FUNC_ARGLIST &&
              generic_func_data->block_ast != nullptr &&
              generic_func_data->block_ast->type == SOLC_AST_TYPE_STMT_BLOCK &&
              generic_func_data->generic_placeholder_type_list_ast != nullptr &&
              generic_func_data->generic_placeholder_type_list_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE_LIST &&
              generic_func_data->name != nullptr);

  string_t header = string_create_from("GENERIC_FUNC { name: \"");
  string_append_cstr(&header, generic_func_data->name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 4);
  if (generic_func_data->type_ast != nullptr) {
    vector_push(children_vs_v,
                ast_get_build_tree_func(generic_func_data->type_ast->type)(
                  generic_func_data->type_ast));
  }
  vector_push(children_vs_v, solc_ast_func_arglist_build_tree(
                               generic_func_data->arg_list_ast));
  vector_push(children_vs_v,
              solc_ast_stmt_block_build_tree(generic_func_data->block_ast));
  vector_push(children_vs_v,
              solc_ast_generic_placeholder_type_list_build_tree(
                generic_func_data->generic_placeholder_type_list_ast));
  return ast_build_tree(&header, children_vs_v);
}

const char *solc_ast_generic_func_get_name(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              (generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC ||
               generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT));
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  SOLC_ASSUME(generic_func_data->name != nullptr);
  return generic_func_data->name;
}

solc_ast_t *solc_ast_generic_func_get_type_ast(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              (generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC ||
               generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT));
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  return generic_func_data->type_ast;
}

solc_ast_t *solc_ast_generic_func_get_arg_list_ast(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              (generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC ||
               generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT));
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  SOLC_ASSUME(generic_func_data->arg_list_ast != nullptr &&
              generic_func_data->arg_list_ast->type ==
                SOLC_AST_TYPE_NONE_FUNC_ARGLIST);
  return generic_func_data->arg_list_ast;
}

solc_ast_t *solc_ast_generic_func_get_block_ast(solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              (generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC ||
               generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT));
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  SOLC_ASSUME(generic_func_data->block_ast != nullptr &&
              generic_func_data->block_ast->type == SOLC_AST_TYPE_STMT_BLOCK);
  return generic_func_data->block_ast;
}

solc_ast_t *solc_ast_generic_func_get_generic_placeholder_type_list_ast(
  solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              (generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC ||
               generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT));
  SOLC_AST_CAST(generic_func_data, generic_func_ast, ast_generic_func_t);
  SOLC_ASSUME(generic_func_data->generic_placeholder_type_list_ast != nullptr &&
              generic_func_data->generic_placeholder_type_list_ast->type ==
                SOLC_AST_TYPE_GENERIC_PLACEHOLDER_TYPE_LIST);
  return generic_func_data->generic_placeholder_type_list_ast;
}
