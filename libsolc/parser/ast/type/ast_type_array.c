#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *size_expr_ast;
  solc_ast_t *type_ast;
} ast_type_array_t;

solc_ast_t *solc_ast_type_array_create(sz pos, solc_ast_t *size_expr_ast,
                                       solc_ast_t *type_ast)
{
  SOLC_ASSUME(type_ast != nullptr);
  ast_type_array_t *out_array_type = malloc(sizeof(ast_type_array_t));
  SOLC_AST_INIT_HEADER(out_array_type, pos, SOLC_AST_TYPE_TYPE_ARRAY);
  out_array_type->size_expr_ast = size_expr_ast;
  out_array_type->type_ast = type_ast;
  return SOLC_AST(out_array_type);
}

void solc_ast_type_array_destroy(solc_ast_t *array_type_ast)
{
  SOLC_ASSUME(array_type_ast != nullptr &&
              array_type_ast->type == SOLC_AST_TYPE_TYPE_ARRAY);
  SOLC_AST_CAST(array_type_data, array_type_ast, ast_type_array_t);
  SOLC_ASSUME(array_type_data->type_ast != nullptr);
  if (array_type_data->size_expr_ast != nullptr) {
    solc_ast_destroy(array_type_data->size_expr_ast);
  }
  solc_ast_destroy(array_type_data->type_ast);
  free(array_type_ast);
}

string_t *solc_ast_type_array_build_tree(solc_ast_t *array_type_ast)
{
  SOLC_ASSUME(array_type_ast != nullptr &&
              array_type_ast->type == SOLC_AST_TYPE_TYPE_ARRAY);
  SOLC_AST_CAST(array_type_data, array_type_ast, ast_type_array_t);
  SOLC_ASSUME(array_type_data->type_ast != nullptr);
  string_t **children_vs_v = vector_create(string_t *);
  vector_push(children_vs_v,
              ast_get_build_tree_func(array_type_data->type_ast->type)(
                array_type_data->type_ast));
  if (array_type_data->size_expr_ast != nullptr) {
    vector_push(children_vs_v,
                ast_get_build_tree_func(array_type_data->size_expr_ast->type)(
                  array_type_data->size_expr_ast));
  }

  string_t header = string_create_from("TYPE_ARRAY");
  return ast_build_tree(&header, children_vs_v);
}
