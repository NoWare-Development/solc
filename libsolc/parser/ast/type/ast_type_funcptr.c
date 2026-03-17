#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
  solc_ast_t *arg_list_ast;
} ast_type_funcptr_t;

solc_ast_t *solc_ast_type_funcptr_create(sz pos, solc_ast_t *type_ast,
                                         solc_ast_t *arg_list_ast)
{
  ast_type_funcptr_t *out_funcptr_type = malloc(sizeof(ast_type_funcptr_t));
  SOLC_AST_INIT_HEADER(out_funcptr_type, pos, SOLC_AST_TYPE_TYPE_FUNCPTR);
  out_funcptr_type->type_ast = type_ast;
  out_funcptr_type->arg_list_ast = arg_list_ast;
  return SOLC_AST(out_funcptr_type);
}

void solc_ast_type_funcptr_destroy(solc_ast_t *funcptr_type_ast)
{
  SOLC_ASSUME(funcptr_type_ast != nullptr &&
              funcptr_type_ast->type == SOLC_AST_TYPE_TYPE_FUNCPTR);
  SOLC_AST_CAST(funcptr_type_data, funcptr_type_ast, ast_type_funcptr_t);
  solc_ast_destroy_if_exists(funcptr_type_data->type_ast);
  solc_ast_destroy_if_exists(funcptr_type_data->arg_list_ast);
  free(funcptr_type_data);
}

string_t *solc_ast_type_funcptr_build_tree(solc_ast_t *funcptr_type_ast)
{
  SOLC_ASSUME(funcptr_type_ast != nullptr &&
              funcptr_type_ast->type == SOLC_AST_TYPE_TYPE_FUNCPTR);
  SOLC_AST_CAST(funcptr_type_data, funcptr_type_ast, ast_type_funcptr_t);
  string_t **children_vs_v = vector_create(string_t *);
  solc_ast_add_to_tree_if_exists(children_vs_v,
                                 funcptr_type_data->arg_list_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, funcptr_type_data->type_ast);
  string_t header = string_create_from("TYPE_FUNCPTR");
  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t *solc_ast_type_funcptr_get_type_ast(solc_ast_t *funcptr_type_ast)
{
  SOLC_ASSUME(funcptr_type_ast != nullptr &&
              funcptr_type_ast->type == SOLC_AST_TYPE_TYPE_FUNCPTR);
  SOLC_AST_CAST(funcptr_type_data, funcptr_type_ast, ast_type_funcptr_t);
  return funcptr_type_data->type_ast;
}

solc_ast_t *solc_ast_type_funcptr_get_arg_list_ast(solc_ast_t *funcptr_type_ast)
{
  SOLC_ASSUME(funcptr_type_ast != nullptr &&
              funcptr_type_ast->type == SOLC_AST_TYPE_TYPE_FUNCPTR);
  SOLC_AST_CAST(funcptr_type_data, funcptr_type_ast, ast_type_funcptr_t);
  return funcptr_type_data->arg_list_ast;
}
