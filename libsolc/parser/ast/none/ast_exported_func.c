#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"

solc_ast_t *solc_ast_exported_func_create(sz pos, const char *name,
                                          solc_ast_t *type_ast,
                                          solc_ast_t *arg_list_ast,
                                          solc_ast_t *block_ast)
{
  solc_ast_t *func_ast =
    solc_ast_func_create(pos, name, type_ast, arg_list_ast, block_ast);
  func_ast->type = SOLC_AST_TYPE_NONE_EXPORTED_FUNC;
  return func_ast;
}

void solc_ast_exported_func_destroy(solc_ast_t *exported_func_ast)
{
  SOLC_ASSUME(exported_func_ast != nullptr &&
              exported_func_ast->type == SOLC_AST_TYPE_NONE_EXPORTED_FUNC);
  exported_func_ast->type = SOLC_AST_TYPE_NONE_FUNC;
  solc_ast_func_destroy(exported_func_ast);
}

string_t *solc_ast_exported_func_build_tree(solc_ast_t *exported_func_ast)
{
  SOLC_ASSUME(exported_func_ast != nullptr &&
              exported_func_ast->type == SOLC_AST_TYPE_NONE_EXPORTED_FUNC);
  const char *name = solc_ast_func_get_name(exported_func_ast);
  solc_ast_t *type_ast = solc_ast_func_get_type_ast(exported_func_ast);
  solc_ast_t *arglist_ast = solc_ast_func_get_arg_list_ast(exported_func_ast);
  solc_ast_t *block_ast = solc_ast_func_get_block_ast(exported_func_ast);

  string_t header = string_create_from("EXPORTED_FUNC { name: \"");
  string_append_cstr(&header, name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 3);
  solc_ast_add_to_tree_if_exists(children_vs_v, type_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, arglist_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, block_ast);

  return ast_build_tree(&header, children_vs_v);
}
