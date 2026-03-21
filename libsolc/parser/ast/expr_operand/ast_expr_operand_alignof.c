#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
} ast_expr_operand_alignof_ast;

solc_ast_t *solc_ast_expr_operand_alignof_create(sz pos, solc_ast_t *expr_ast)
{
  ast_expr_operand_alignof_ast *out_expr_operand_alignof =
    malloc(sizeof(ast_expr_operand_alignof_ast));
  SOLC_AST_INIT_HEADER(out_expr_operand_alignof, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_ALIGNOF);
  out_expr_operand_alignof->expr_ast = expr_ast;
  return SOLC_AST(out_expr_operand_alignof);
}

void solc_ast_expr_operand_alignof_destroy(solc_ast_t *alignof_expr_operand_ast)
{
  SOLC_ASSUME(alignof_expr_operand_ast != nullptr &&
              alignof_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_ALIGNOF);
  SOLC_AST_CAST(alignof_expr_operand_data, alignof_expr_operand_ast,
                ast_expr_operand_alignof_ast);
  solc_ast_destroy_if_exists(alignof_expr_operand_data->expr_ast);
  free(alignof_expr_operand_data);
}

string_t *
solc_ast_expr_operand_alignof_build_tree(solc_ast_t *alignof_expr_operand_ast)
{
  SOLC_ASSUME(alignof_expr_operand_ast != nullptr &&
              alignof_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_ALIGNOF);
  SOLC_AST_CAST(alignof_expr_operand_data, alignof_expr_operand_ast,
                ast_expr_operand_alignof_ast);
  string_t header = string_create_from("EXPR_OPERAND_ALIGNOF");
  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v,
                                 alignof_expr_operand_data->expr_ast);
  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t *
solc_ast_expr_operand_alignof_get_expr_ast(solc_ast_t *alignof_expr_operand_ast)
{
  SOLC_ASSUME(alignof_expr_operand_ast != nullptr &&
              alignof_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_ALIGNOF);
  SOLC_AST_CAST(alignof_expr_operand_data, alignof_expr_operand_ast,
                ast_expr_operand_alignof_ast);
  return alignof_expr_operand_data->expr_ast;
}
