#include "containers/vector.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"

solc_ast_t *solc_ast_func_explicit_create(solc_ast_t *func_ast)
{
  SOLC_ASSUME(func_ast != nullptr && func_ast->type == SOLC_AST_TYPE_NONE_FUNC);
  func_ast->type = SOLC_AST_TYPE_NONE_FUNC_EXPLICIT;
  return func_ast;
}

void solc_ast_func_explicit_destroy(solc_ast_t *func_explicit_ast)
{
  SOLC_ASSUME(func_explicit_ast != nullptr &&
              func_explicit_ast->type == SOLC_AST_TYPE_NONE_FUNC_EXPLICIT);
  func_explicit_ast->type = SOLC_AST_TYPE_NONE_FUNC;
  solc_ast_func_destroy(func_explicit_ast);
}

string_t *solc_ast_func_explicit_build_tree(solc_ast_t *func_explicit_ast)
{
  SOLC_ASSUME(func_explicit_ast != nullptr &&
              func_explicit_ast->type == SOLC_AST_TYPE_NONE_FUNC_EXPLICIT);
  const char *name = solc_ast_func_get_name(func_explicit_ast);
  solc_ast_t *type_ast = solc_ast_func_get_type_ast(func_explicit_ast);
  solc_ast_t *arglist_ast = solc_ast_func_get_arg_list_ast(func_explicit_ast);
  solc_ast_t *block_ast = solc_ast_func_get_block_ast(func_explicit_ast);

  string_t header = string_create_from("EXTERN_FUNC { name: \"");
  string_append_cstr(&header, name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 3);
  if (type_ast != nullptr) {
    vector_push(children_vs_v,
                ast_get_build_tree_func(type_ast->type)(type_ast));
  }
  vector_push(children_vs_v, solc_ast_func_arglist_build_tree(arglist_ast));
  vector_push(children_vs_v, solc_ast_stmt_block_build_tree(block_ast));

  return ast_build_tree(&header, children_vs_v);
}
