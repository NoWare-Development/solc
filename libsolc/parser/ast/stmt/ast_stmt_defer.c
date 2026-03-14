#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *stmt_ast;
} ast_defer_stmt_t;

solc_ast_t *solc_ast_stmt_defer_create(sz pos, solc_ast_t *stmt_ast)
{
  ast_defer_stmt_t *out_defer_stmt = malloc(sizeof(ast_defer_stmt_t));
  SOLC_AST_INIT_HEADER(out_defer_stmt, pos, SOLC_AST_TYPE_STMT_DEFER);
  out_defer_stmt->stmt_ast = stmt_ast;
  return SOLC_AST(out_defer_stmt);
}

void solc_ast_stmt_defer_destroy(solc_ast_t *defer_ast)
{
  SOLC_ASSUME(defer_ast != nullptr &&
              defer_ast->type == SOLC_AST_TYPE_STMT_DEFER);
  SOLC_AST_CAST(defer_data, defer_ast, ast_defer_stmt_t);
  solc_ast_destroy_if_exists(defer_data->stmt_ast);
  free(defer_data);
}

string_t *solc_ast_stmt_defer_build_tree(solc_ast_t *defer_ast)
{
  SOLC_ASSUME(defer_ast != nullptr &&
              defer_ast->type == SOLC_AST_TYPE_STMT_DEFER);
  SOLC_AST_CAST(defer_data, defer_ast, ast_defer_stmt_t);

  string_t header = string_create_from("STMT_DEFER");
  string_t **children_vs_v = vector_reserve(string_t *, 1);
  solc_ast_add_to_tree_if_exists(children_vs_v, defer_data->stmt_ast);

  return ast_build_tree(&header, children_vs_v);
}
