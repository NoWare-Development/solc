#include "containers/string.h"
#include "parser/ast_private.h"

#include "solc/parser/ast.h"
#include <stdlib.h>
#include "containers/vector.h"

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **top_stmts_v;
} ast_root_t;

solc_ast_t *solc_ast_root_create(void)
{
  ast_root_t *out_root = malloc(sizeof(ast_root_t));
  SOLC_AST_INIT_HEADER(out_root, 0, SOLC_AST_TYPE_NONE_ROOT);
  out_root->top_stmts_v = vector_create(solc_ast_t *);
  return SOLC_AST(out_root);
}

void solc_ast_root_destroy(solc_ast_t *root_ast)
{
  SOLC_ASSUME(root_ast != nullptr && root_ast->type == SOLC_AST_TYPE_NONE_ROOT);

  SOLC_AST_CAST(root_data, root_ast, ast_root_t);
  SOLC_ASSUME(root_data->top_stmts_v != nullptr);
  for (sz i = 0, top_stmts_v_size = vector_get_length(root_data->top_stmts_v);
       i < top_stmts_v_size; i++)
    solc_ast_destroy_if_exists(root_data->top_stmts_v[i]);
  vector_destroy(root_data->top_stmts_v);
  free(root_data);
}

void solc_ast_root_add_top_statement(solc_ast_t *root_ast,
                                     solc_ast_t *top_stmt_ast)
{
  SOLC_ASSUME(root_ast != nullptr && root_ast->type == SOLC_AST_TYPE_NONE_ROOT);
  SOLC_AST_CAST(root_data, root_ast, ast_root_t);
  SOLC_ASSUME(root_data->top_stmts_v != nullptr);
  vector_push(root_data->top_stmts_v, top_stmt_ast);
}

string_t *solc_ast_root_build_tree(solc_ast_t *root_ast)
{
  SOLC_ASSUME(root_ast != nullptr && root_ast->type == SOLC_AST_TYPE_NONE_ROOT);
  SOLC_AST_CAST(root_data, root_ast, ast_root_t);
  SOLC_ASSUME(root_data->top_stmts_v != nullptr);
  sz top_stmts_v_len = vector_get_length(root_data->top_stmts_v);
  string_t **children_vs_v = vector_reserve(string_t *, top_stmts_v_len);
  for (sz i = 0; i < top_stmts_v_len; i++)
    solc_ast_add_to_tree_if_exists(children_vs_v, root_data->top_stmts_v[i]);
  string_t header = string_create_from("ROOT");
  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t **solc_ast_root_get_top_statements(solc_ast_t *root_ast, sz *out_n)
{
  SOLC_ASSUME(root_ast != nullptr && root_ast->type == SOLC_AST_TYPE_NONE_ROOT);
  SOLC_AST_CAST(root_data, root_ast, ast_root_t);
  SOLC_ASSUME(root_data->top_stmts_v != nullptr);
  if SOLC_LIKELY (out_n != nullptr)
    *out_n = vector_get_length(root_data->top_stmts_v);
  return root_data->top_stmts_v;
}
