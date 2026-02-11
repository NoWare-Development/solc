#include "parser/ast_private.h"
#include "containers/vector.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
  solc_ast_t **case_asts_v;
} ast_switch_stmt_t;

solc_ast_t *solc_ast_stmt_switch_create(sz pos, solc_ast_t *expr_ast)
{
  SOLC_ASSUME(expr_ast != nullptr);
  ast_switch_stmt_t *out_switch_stmt = malloc(sizeof(ast_switch_stmt_t));
  SOLC_AST_INIT_HEADER(out_switch_stmt, pos, SOLC_AST_TYPE_STMT_SWITCH);
  out_switch_stmt->expr_ast = expr_ast;
  out_switch_stmt->case_asts_v = vector_create(solc_ast_t *);
  return SOLC_AST(out_switch_stmt);
}

void solc_ast_stmt_switch_destroy(solc_ast_t *switch_ast)
{
  SOLC_ASSUME(switch_ast != nullptr &&
              switch_ast->type == SOLC_AST_TYPE_STMT_SWITCH);
  SOLC_AST_CAST(switch_data, switch_ast, ast_switch_stmt_t);
  SOLC_ASSUME(switch_data->expr_ast != nullptr &&
              switch_data->case_asts_v != nullptr);
  solc_ast_destroy(switch_data->expr_ast);
  for (sz i = 0, case_asts_v_size = vector_get_length(switch_data->case_asts_v);
       i < case_asts_v_size; i++) {
    SOLC_ASSUME(switch_data->case_asts_v[i] != nullptr);
    solc_ast_destroy(switch_data->case_asts_v[i]);
  }
  vector_destroy(switch_data->case_asts_v);
  free(switch_data);
}

void solc_ast_stmt_switch_add_case(solc_ast_t *switch_ast, solc_ast_t *case_ast)
{
  SOLC_ASSUME(switch_ast != nullptr &&
              switch_ast->type == SOLC_AST_TYPE_STMT_SWITCH &&
              case_ast != nullptr);
  SOLC_AST_CAST(switch_data, switch_ast, ast_switch_stmt_t);
  SOLC_ASSUME(switch_data->case_asts_v != nullptr);
  vector_push(switch_data->case_asts_v, case_ast);
}

sz solc_ast_stmt_switch_to_string(char *buf, sz n, solc_ast_t *switch_ast)
{
  SOLC_ASSUME(buf != nullptr && switch_ast != nullptr &&
              switch_ast->type == SOLC_AST_TYPE_STMT_SWITCH);

  SOLC_TODO("Switch statement to string.");

  return 0;
}
