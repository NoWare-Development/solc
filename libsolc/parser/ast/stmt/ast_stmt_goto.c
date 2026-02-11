#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *label_name;
} ast_goto_stmt_t;

solc_ast_t *solc_ast_stmt_goto_create(sz pos, const char *label_name)
{
  SOLC_ASSUME(label_name != nullptr);
  const sz label_name_len = strlen(label_name);
  ast_goto_stmt_t *out_goto_stmt =
    malloc(sizeof(ast_goto_stmt_t) + label_name_len);
  SOLC_AST_INIT_HEADER(out_goto_stmt, pos, SOLC_AST_TYPE_STMT_GOTO);
  out_goto_stmt->label_name = (char *)out_goto_stmt + sizeof(ast_goto_stmt_t);
  memcpy(out_goto_stmt->label_name, label_name, label_name_len);
  return SOLC_AST(out_goto_stmt);
}

void solc_ast_stmt_goto_destroy(solc_ast_t *goto_ast)
{
  SOLC_ASSUME(goto_ast != nullptr && goto_ast->type == SOLC_AST_TYPE_STMT_GOTO);
  free(goto_ast);
}

sz solc_ast_stmt_goto_to_string(char *buf, sz n, solc_ast_t *goto_ast)
{
  SOLC_ASSUME(buf != nullptr && goto_ast != nullptr &&
              goto_ast->type == SOLC_AST_TYPE_STMT_GOTO);
  SOLC_AST_CAST(goto_data, goto_ast, ast_goto_stmt_t);
  SOLC_ASSUME(goto_data->label_name != nullptr);
  return snprintf(buf, n, "STMT_GOTO { name: \"%s\" }", goto_data->label_name);
}
