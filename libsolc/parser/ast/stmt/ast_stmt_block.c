#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **stmt_asts_v;
} ast_block_stmt_t;

solc_ast_t *solc_ast_stmt_block_create(sz pos)
{
  ast_block_stmt_t *out_block_stmt = malloc(sizeof(ast_block_stmt_t));
  SOLC_AST_INIT_HEADER(out_block_stmt, pos, SOLC_AST_TYPE_STMT_BLOCK);
  out_block_stmt->stmt_asts_v = vector_create(solc_ast_t *);
  return SOLC_AST(out_block_stmt);
}

void solc_ast_stmt_block_destroy(solc_ast_t *block_ast)
{
  SOLC_ASSUME(block_ast != nullptr &&
              block_ast->type == SOLC_AST_TYPE_STMT_BLOCK);
  SOLC_AST_CAST(block_data, block_ast, ast_block_stmt_t);
  SOLC_ASSUME(block_data->stmt_asts_v != nullptr);
  for (sz i = 0, stmt_asts_v_size = vector_get_length(block_data->stmt_asts_v);
       i < stmt_asts_v_size; i++) {
    SOLC_ASSUME(block_data->stmt_asts_v[i] != nullptr);
    solc_ast_destroy(block_data->stmt_asts_v[i]);
  }
  vector_destroy(block_data->stmt_asts_v);
  free(block_ast);
}

void solc_ast_stmt_block_add_stmt(solc_ast_t *block_ast, solc_ast_t *stmt_ast)
{
  SOLC_ASSUME(block_ast != nullptr &&
              block_ast->type == SOLC_AST_TYPE_STMT_BLOCK &&
              stmt_ast != nullptr);
  SOLC_AST_CAST(block_data, block_ast, ast_block_stmt_t);
  SOLC_ASSUME(block_data->stmt_asts_v != nullptr);
  vector_push(block_data->stmt_asts_v, stmt_ast);
}

sz solc_ast_stmt_block_to_string(char *buf, sz n, solc_ast_t *block_ast)
{
  SOLC_ASSUME(buf != nullptr && block_ast != nullptr &&
              block_ast->type == SOLC_AST_TYPE_STMT_BLOCK);

  SOLC_TODO("Block to string.");

  return 0;
}
