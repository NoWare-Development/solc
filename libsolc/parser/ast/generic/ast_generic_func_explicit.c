#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast/ast_group_generic.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast/ast_group_stmt.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"

solc_ast_t *solc_ast_generic_func_explicit_create(sz pos,
                                                  solc_ast_t *generic_func_ast)
{
  SOLC_ASSUME(generic_func_ast != nullptr &&
              generic_func_ast->type == SOLC_AST_TYPE_GENERIC_FUNC);
  generic_func_ast->type = SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT;
  return generic_func_ast;
}

void solc_ast_generic_func_explicit_destroy(
  solc_ast_t *generic_func_explicit_ast)
{
  SOLC_ASSUME(generic_func_explicit_ast != nullptr &&
              generic_func_explicit_ast->type ==
                SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT);
  generic_func_explicit_ast->type = SOLC_AST_TYPE_GENERIC_FUNC;
  solc_ast_generic_func_destroy(generic_func_explicit_ast);
}

string_t *
solc_ast_generic_func_explicit_build_tree(solc_ast_t *generic_func_explicit_ast)
{
  SOLC_ASSUME(generic_func_explicit_ast != nullptr &&
              generic_func_explicit_ast->type ==
                SOLC_AST_TYPE_GENERIC_FUNC_EXPLICIT);
  const char *name = solc_ast_generic_func_get_name(generic_func_explicit_ast);
  solc_ast_t *type_ast =
    solc_ast_generic_func_get_type_ast(generic_func_explicit_ast);
  solc_ast_t *arg_list_ast =
    solc_ast_generic_func_get_arg_list_ast(generic_func_explicit_ast);
  solc_ast_t *block_ast =
    solc_ast_generic_func_get_block_ast(generic_func_explicit_ast);
  solc_ast_t *generic_placeholder_type_list_ast =
    solc_ast_generic_func_get_generic_placeholder_type_list_ast(
      generic_func_explicit_ast);

  string_t header = string_create_from("GENERIC_FUNC_EXPLICIT { name: \"");
  string_append_cstr(&header, name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 4);
  if (type_ast != nullptr) {
    vector_push(children_vs_v,
                ast_get_build_tree_func(type_ast->type)(type_ast));
  }
  vector_push(children_vs_v, solc_ast_generic_placeholder_type_list_build_tree(
                               generic_placeholder_type_list_ast));
  vector_push(children_vs_v, solc_ast_func_arglist_build_tree(arg_list_ast));
  vector_push(children_vs_v, solc_ast_stmt_block_build_tree(block_ast));

  return ast_build_tree(&header, children_vs_v);
}
