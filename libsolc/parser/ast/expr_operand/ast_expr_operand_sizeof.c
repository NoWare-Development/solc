#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
} ast_expr_operand_sizeof_t;

solc_ast_t *solc_ast_expr_operand_sizeof_create(sz pos, solc_ast_t *type_ast)
{
  SOLC_ASSUME(type_ast != nullptr);
  ast_expr_operand_sizeof_t *out_expr_operand_sizeof =
    malloc(sizeof(ast_expr_operand_sizeof_t));
  SOLC_AST_INIT_HEADER(out_expr_operand_sizeof, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_SIZEOF);
  out_expr_operand_sizeof->type_ast = type_ast;
  return SOLC_AST(out_expr_operand_sizeof);
}

void solc_ast_expr_operand_sizeof_destroy(solc_ast_t *sizeof_expr_operand_ast)
{
  SOLC_ASSUME(sizeof_expr_operand_ast != nullptr &&
              sizeof_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_SIZEOF);
  SOLC_AST_CAST(sizeof_expr_operand_data, sizeof_expr_operand_ast,
                ast_expr_operand_sizeof_t);
  SOLC_ASSUME(sizeof_expr_operand_data->type_ast != nullptr);
  solc_ast_destroy(sizeof_expr_operand_data->type_ast);
  free(sizeof_expr_operand_data);
}

string_t *
solc_ast_expr_operand_sizeof_build_tree(solc_ast_t *sizeof_expr_operand_ast)
{
  SOLC_ASSUME(sizeof_expr_operand_ast != nullptr &&
              sizeof_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_SIZEOF);
  SOLC_AST_CAST(sizeof_expr_operand_data, sizeof_expr_operand_ast,
                ast_expr_operand_sizeof_t);
  SOLC_ASSUME(sizeof_expr_operand_data->type_ast != nullptr);
  string_t header = string_create_from("EXPR_OPERAND_SIZEOF");
  string_t **children_vs_v = vector_reserve(string_t *, 1);
  vector_push(children_vs_v,
              ast_get_build_tree_func(sizeof_expr_operand_data->type_ast->type)(
                sizeof_expr_operand_data->type_ast));

  return ast_build_tree(&header, children_vs_v);
}
