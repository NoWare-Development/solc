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
       i < top_stmts_v_size; i++) {
    SOLC_ASSUME(root_data->top_stmts_v[i] != nullptr);
    solc_ast_destroy(root_data->top_stmts_v[i]);
  }
  vector_destroy(root_data->top_stmts_v);
  free(root_data);
}

void solc_ast_root_add_top_statement(solc_ast_t *root_ast,
                                     solc_ast_t *top_stmt_ast)
{
  SOLC_ASSUME(root_ast != nullptr &&
              root_ast->type == SOLC_AST_TYPE_NONE_ROOT &&
              top_stmt_ast != nullptr);

  SOLC_AST_CAST(root_data, root_ast, ast_root_t);
  SOLC_ASSUME(root_data->top_stmts_v != nullptr);
  vector_push(root_data->top_stmts_v, top_stmt_ast);
}

sz solc_ast_root_to_string(char *buf, sz n, solc_ast_t *root_ast)
{
  SOLC_ASSUME(buf != nullptr && root_ast != nullptr &&
              root_ast->type == SOLC_AST_TYPE_NONE_ROOT);

  SOLC_TODO("Root AST to string");
  return 0;
}
