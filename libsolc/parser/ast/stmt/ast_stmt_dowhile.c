#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *condition_expr_ast;
  solc_ast_t *stmt_ast;
} ast_dowhile_t;

solc_ast_t *solc_ast_stmt_dowhile_create(sz pos, solc_ast_t *condition_expr_ast,
                                         solc_ast_t *stmt_ast)
{
  ast_dowhile_t *out_dowhile_ast = malloc(sizeof(ast_dowhile_t));
  SOLC_AST_INIT_HEADER(out_dowhile_ast, pos, SOLC_AST_TYPE_STMT_DOWHILE);
  out_dowhile_ast->condition_expr_ast = condition_expr_ast;
  out_dowhile_ast->stmt_ast = stmt_ast;
  return SOLC_AST(out_dowhile_ast);
}

void solc_ast_stmt_dowhile_destroy(solc_ast_t *dowhile_ast)
{
  SOLC_ASSUME(dowhile_ast != nullptr &&
              dowhile_ast->type == SOLC_AST_TYPE_STMT_DOWHILE);
  SOLC_AST_CAST(dowhile_data, dowhile_ast, ast_dowhile_t);
  solc_ast_destroy_if_exists(dowhile_data->condition_expr_ast);
  solc_ast_destroy_if_exists(dowhile_data->stmt_ast);
  free(dowhile_data);
}

string_t *solc_ast_stmt_dowhile_build_tree(solc_ast_t *dowhile_ast)
{
  SOLC_ASSUME(dowhile_ast != nullptr &&
              dowhile_ast->type == SOLC_AST_TYPE_STMT_DOWHILE);
  SOLC_AST_CAST(dowhile_data, dowhile_ast, ast_dowhile_t);
  string_t header = string_create_from("STMT_DOWHILE");
  string_t **children_vs_v = vector_reserve(string_t *, 2);
  solc_ast_add_to_tree_if_exists(children_vs_v,
                                 dowhile_data->condition_expr_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, dowhile_data->stmt_ast);

  return ast_build_tree(&header, children_vs_v);
}
