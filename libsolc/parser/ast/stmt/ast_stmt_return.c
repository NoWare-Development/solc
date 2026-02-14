#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
} ast_return_stmt_t;

solc_ast_t *solc_ast_stmt_return_create(sz pos, solc_ast_t *expr_ast)
{
  ast_return_stmt_t *out_return_stmt = malloc(sizeof(ast_return_stmt_t));
  SOLC_AST_INIT_HEADER(out_return_stmt, pos, SOLC_AST_TYPE_STMT_RETURN);
  out_return_stmt->expr_ast = expr_ast;
  return SOLC_AST(out_return_stmt);
}

void solc_ast_stmt_return_destroy(solc_ast_t *return_ast)
{
  SOLC_ASSUME(return_ast != nullptr &&
              return_ast->type == SOLC_AST_TYPE_STMT_RETURN);
  SOLC_AST_CAST(return_data, return_ast, ast_return_stmt_t);
  if (return_data->expr_ast != nullptr) {
    solc_ast_destroy(return_data->expr_ast);
  }
  free(return_ast);
}

string_t *solc_ast_stmt_return_build_tree(solc_ast_t *return_ast)
{
  SOLC_ASSUME(return_ast != nullptr &&
              return_ast->type == SOLC_AST_TYPE_STMT_RETURN);
  SOLC_AST_CAST(return_data, return_ast, ast_return_stmt_t);

  string_t header = string_create_from("STMT_RETURN");
  if (return_data->expr_ast == nullptr) {
    string_t *out_v = vector_reserve(string_t, 1);
    vector_push(out_v, header);
    return out_v;
  }

  string_t **children_vs_v = vector_reserve(string_t *, 1);
  vector_push(children_vs_v,
              ast_get_build_tree_func(return_data->expr_ast->type)(
                return_data->expr_ast));

  return ast_build_tree(&header, children_vs_v);
}
