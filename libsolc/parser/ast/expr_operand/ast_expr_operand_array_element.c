#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
  solc_ast_t *parent_ast;
} ast_expr_operand_array_element_t;

solc_ast_t *solc_ast_expr_operand_array_element_create(sz pos,
                                                       solc_ast_t *expr_ast,
                                                       solc_ast_t *parent_ast)
{
  SOLC_ASSUME(expr_ast != nullptr && parent_ast != nullptr);
  ast_expr_operand_array_element_t *out_expr_operand_array_element =
    malloc(sizeof(ast_expr_operand_array_element_t));
  SOLC_AST_INIT_HEADER(out_expr_operand_array_element, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_ARRAY_ELEMENT);
  out_expr_operand_array_element->expr_ast = expr_ast;
  out_expr_operand_array_element->parent_ast = parent_ast;
  return SOLC_AST(out_expr_operand_array_element);
}

void solc_ast_expr_operand_array_element_destroy(
  solc_ast_t *array_element_expr_operand_ast)
{
  SOLC_ASSUME(array_element_expr_operand_ast != nullptr &&
              array_element_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_ARRAY_ELEMENT);
  SOLC_AST_CAST(array_element_expr_operand_data, array_element_expr_operand_ast,
                ast_expr_operand_array_element_t);
  SOLC_ASSUME(array_element_expr_operand_data->expr_ast != nullptr &&
              array_element_expr_operand_data->parent_ast != nullptr);
  solc_ast_destroy(array_element_expr_operand_data->expr_ast);
  solc_ast_destroy(array_element_expr_operand_data->parent_ast);
  free(array_element_expr_operand_data);
}

string_t *solc_ast_expr_operand_array_element_build_tree(
  solc_ast_t *array_element_expr_operand_ast)
{
  SOLC_ASSUME(array_element_expr_operand_ast != nullptr &&
              array_element_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_ARRAY_ELEMENT);
  SOLC_AST_CAST(array_element_expr_operand_data, array_element_expr_operand_ast,
                ast_expr_operand_array_element_t);
  SOLC_ASSUME(array_element_expr_operand_data->expr_ast != nullptr &&
              array_element_expr_operand_data->parent_ast != nullptr);
  string_t **children_vs_v = vector_create(string_t *);
  vector_push(children_vs_v, ast_get_build_tree_func(
                               array_element_expr_operand_data->expr_ast->type)(
                               array_element_expr_operand_data->expr_ast));
  vector_push(
    children_vs_v,
    ast_get_build_tree_func(array_element_expr_operand_data->parent_ast->type)(
      array_element_expr_operand_data->parent_ast));
  string_t header = string_create_from("EXPR_OPERAND_ARRAY_ELEMENT");
  return ast_build_tree(&header, children_vs_v);
}
