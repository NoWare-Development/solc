#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *index_expr_ast;
  solc_ast_t *parent_ast;
} ast_expr_operand_array_element_t;

solc_ast_t *
solc_ast_expr_operand_array_element_create(sz pos, solc_ast_t *index_expr_ast,
                                           solc_ast_t *parent_ast)
{
  ast_expr_operand_array_element_t *out_expr_operand_array_element =
    malloc(sizeof(ast_expr_operand_array_element_t));
  SOLC_AST_INIT_HEADER(out_expr_operand_array_element, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_ARRAY_ELEMENT);
  out_expr_operand_array_element->index_expr_ast = index_expr_ast;
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
  solc_ast_destroy_if_exists(array_element_expr_operand_data->index_expr_ast);
  solc_ast_destroy_if_exists(array_element_expr_operand_data->parent_ast);
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
  string_t **children_vs_v = vector_create(string_t *);
  solc_ast_add_to_tree_if_exists(
    children_vs_v, array_element_expr_operand_data->index_expr_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v,
                                 array_element_expr_operand_data->parent_ast);

  string_t header = string_create_from("EXPR_OPERAND_ARRAY_ELEMENT");
  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t *solc_ast_expr_operand_array_element_get_index_expr_ast(
  solc_ast_t *array_element_expr_operand_ast)
{
  SOLC_ASSUME(array_element_expr_operand_ast != nullptr &&
              array_element_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_ARRAY_ELEMENT);
  SOLC_AST_CAST(array_element_expr_operand_data, array_element_expr_operand_ast,
                ast_expr_operand_array_element_t);
  return array_element_expr_operand_data->index_expr_ast;
}

solc_ast_t *solc_ast_expr_operand_array_element_get_parent_ast(
  solc_ast_t *array_element_expr_operand_ast)
{
  SOLC_ASSUME(array_element_expr_operand_ast != nullptr &&
              array_element_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_ARRAY_ELEMENT);
  SOLC_AST_CAST(array_element_expr_operand_data, array_element_expr_operand_ast,
                ast_expr_operand_array_element_t);
  return array_element_expr_operand_data->parent_ast;
}
