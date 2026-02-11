#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>

solc_ast_t *solc_ast_stmt_break_create(sz pos)
{
  solc_ast_t *out_break_stmt = malloc(sizeof(solc_ast_t));
  out_break_stmt->token_pos = pos;
  out_break_stmt->type = SOLC_AST_TYPE_STMT_BREAK;
  return out_break_stmt;
}

void solc_ast_stmt_break_destroy(solc_ast_t *break_ast)
{
  SOLC_ASSUME(break_ast != nullptr &&
              break_ast->type == SOLC_AST_TYPE_STMT_BREAK);
  free(break_ast);
}

sz solc_ast_stmt_break_to_string(char *buf, sz n, solc_ast_t *break_ast)
{
  SOLC_ASSUME(buf != nullptr && break_ast != nullptr &&
              break_ast->type == SOLC_AST_TYPE_STMT_BREAK);
  return snprintf(buf, n, "STMT_BREAK");
}
