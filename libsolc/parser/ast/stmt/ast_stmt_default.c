#include "parser/ast/ast_group_stmt.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *block_ast;
} ast_default_stmt_t;

solc_ast_t *solc_ast_stmt_default_create(sz pos, solc_ast_t *block_ast)
{
  SOLC_ASSUME(block_ast != nullptr &&
              block_ast->type == SOLC_AST_TYPE_STMT_BLOCK);
  ast_default_stmt_t *out_default_stmt = malloc(sizeof(ast_default_stmt_t));
  SOLC_AST_INIT_HEADER(out_default_stmt, pos, SOLC_AST_TYPE_STMT_DEFAULT);
  out_default_stmt->block_ast = block_ast;
  return SOLC_AST(out_default_stmt);
}

void solc_ast_stmt_default_destroy(solc_ast_t *default_ast)
{
  SOLC_ASSUME(default_ast != nullptr &&
              default_ast->type == SOLC_AST_TYPE_STMT_DEFAULT);
  SOLC_AST_CAST(default_data, default_ast, ast_default_stmt_t);
  SOLC_ASSUME(default_data->block_ast != nullptr &&
              default_data->block_ast->type == SOLC_AST_TYPE_STMT_BLOCK);
  solc_ast_stmt_block_destroy(default_data->block_ast);
  free(default_data);
}

sz solc_ast_stmt_default_to_string(char *buf, sz n, solc_ast_t *default_ast)
{
  SOLC_ASSUME(buf != nullptr && default_ast != nullptr &&
              default_ast->type == SOLC_AST_TYPE_STMT_DEFAULT);

  SOLC_TODO("Default statement to string.");

  return 0;
}
