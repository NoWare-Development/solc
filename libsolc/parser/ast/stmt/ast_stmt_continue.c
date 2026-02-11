#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>

solc_ast_t *solc_ast_stmt_continue_create(sz pos)
{
  solc_ast_t *out_continue_stmt = malloc(sizeof(solc_ast_t));
  out_continue_stmt->token_pos = pos;
  out_continue_stmt->type = SOLC_AST_TYPE_STMT_CONTINUE;
  return out_continue_stmt;
}

void solc_ast_stmt_continue_destroy(solc_ast_t *continue_ast)
{
  SOLC_ASSUME(continue_ast != nullptr &&
              continue_ast->type == SOLC_AST_TYPE_STMT_CONTINUE);
  free(continue_ast);
}

sz solc_ast_stmt_continue_to_string(char *buf, sz n, solc_ast_t *continue_ast)
{
  SOLC_ASSUME(buf != nullptr && continue_ast != nullptr &&
              continue_ast->type == SOLC_AST_TYPE_STMT_CONTINUE);
  return snprintf(buf, n, "STMT_CONTINUE");
}
