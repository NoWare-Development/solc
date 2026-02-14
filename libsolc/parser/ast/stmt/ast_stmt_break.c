#include "containers/string.h"
#include "containers/vector.h"
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

string_t *solc_ast_stmt_break_build_tree(solc_ast_t *break_ast)
{
  SOLC_ASSUME(break_ast != nullptr &&
              break_ast->type == SOLC_AST_TYPE_STMT_BREAK);
  string_t *out_v = vector_reserve(string_t, 1);
  vector_push(out_v, string_create_from("STMT_BREAK"));
  return out_v;
}
