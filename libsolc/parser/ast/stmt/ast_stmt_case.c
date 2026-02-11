#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
  solc_ast_t *block_ast;
} ast_case_stmt_t;

solc_ast_t *solc_ast_stmt_case_create(sz pos, solc_ast_t *expr_ast,
                                      solc_ast_t *block_ast)
{
  SOLC_ASSUME(expr_ast != nullptr && block_ast != nullptr &&
              block_ast->type == SOLC_AST_TYPE_STMT_BLOCK);
  ast_case_stmt_t *out_case_stmt = malloc(sizeof(ast_case_stmt_t));
  SOLC_AST_INIT_HEADER(out_case_stmt, pos, SOLC_AST_TYPE_STMT_CASE);
  out_case_stmt->expr_ast = expr_ast;
  out_case_stmt->block_ast = block_ast;
  return SOLC_AST(out_case_stmt);
}

void solc_ast_stmt_case_destroy(solc_ast_t *case_ast)
{
  SOLC_ASSUME(case_ast != nullptr && case_ast->type == SOLC_AST_TYPE_STMT_CASE);
  SOLC_AST_CAST(case_data, case_ast, ast_case_stmt_t);
  SOLC_ASSUME(case_data->expr_ast != nullptr &&
              case_data->block_ast != nullptr &&
              case_data->block_ast->type == SOLC_AST_TYPE_STMT_BLOCK);
  solc_ast_destroy(case_data->expr_ast);
  solc_ast_stmt_block_destroy(case_data->block_ast);
  free(case_ast);
}

sz solc_ast_stmt_case_to_string(char *buf, sz n, solc_ast_t *case_ast)
{
  SOLC_ASSUME(buf != nullptr && case_ast != nullptr &&
              case_ast->type == SOLC_AST_TYPE_STMT_CASE);

  SOLC_TODO("Case statement to string.");

  return 0;
}
