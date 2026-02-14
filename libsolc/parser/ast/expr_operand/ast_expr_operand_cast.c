#include "solc/parser/ast.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
  solc_ast_t *expr_ast;
} ast_expr_operand_cast_t;

solc_ast_t *solc_ast_expr_operand_cast_to_create(sz pos, solc_ast_t *type_ast,
                                                 solc_ast_t *expr_ast)
{
  SOLC_ASSUME(type_ast != nullptr && expr_ast != nullptr);
  ast_expr_operand_cast_t *out_expr_operand_cast =
    malloc(sizeof(ast_expr_operand_cast_t));
  SOLC_AST_INIT_HEADER(out_expr_operand_cast, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_CAST_TO);
  out_expr_operand_cast->type_ast = type_ast;
  out_expr_operand_cast->expr_ast = expr_ast;
  return SOLC_AST(out_expr_operand_cast);
}

void solc_ast_expr_operand_cast_to_destroy(solc_ast_t *cast_to_expr_operand_ast)
{
  SOLC_ASSUME(cast_to_expr_operand_ast != nullptr &&
              cast_to_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_CAST_TO);
  SOLC_AST_CAST(cast_to_expr_operand_data, cast_to_expr_operand_ast,
                ast_expr_operand_cast_t);
  SOLC_ASSUME(cast_to_expr_operand_data->type_ast != nullptr &&
              cast_to_expr_operand_data->expr_ast != nullptr);
  solc_ast_destroy(cast_to_expr_operand_data->type_ast);
  solc_ast_destroy(cast_to_expr_operand_data->expr_ast);
  free(cast_to_expr_operand_ast);
}

string_t *
solc_ast_expr_operand_cast_to_build_tree(solc_ast_t *cast_to_expr_operand_ast)
{
  SOLC_ASSUME(cast_to_expr_operand_ast != nullptr &&
              cast_to_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_CAST_TO);
  SOLC_AST_CAST(cast_to_expr_operand_data, cast_to_expr_operand_ast,
                ast_expr_operand_cast_t);
  SOLC_ASSUME(cast_to_expr_operand_data->type_ast != nullptr &&
              cast_to_expr_operand_data->expr_ast != nullptr);

  solc_ast_build_tree_func_t type_build_tree_func =
    ast_get_build_tree_func(cast_to_expr_operand_data->type_ast->type);
  solc_ast_build_tree_func_t expr_build_tree_func =
    ast_get_build_tree_func(cast_to_expr_operand_data->expr_ast->type);

  string_t **children_vs_v = vector_reserve(string_t *, 2);
  vector_push(children_vs_v,
              type_build_tree_func(cast_to_expr_operand_data->type_ast));
  vector_push(children_vs_v,
              expr_build_tree_func(cast_to_expr_operand_data->expr_ast));

  string_t heading = string_create_from("EXPR_OPERAND_CAST_TO");
  return ast_build_tree(&heading, children_vs_v);
}
