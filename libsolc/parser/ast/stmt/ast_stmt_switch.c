#include "containers/string.h"
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
  SOLC_ASSUME(switch_data->case_asts_v != nullptr);
  solc_ast_destroy_if_exists(switch_data->expr_ast);
  for (sz i = 0, case_asts_v_size = vector_get_length(switch_data->case_asts_v);
       i < case_asts_v_size; i++)
    solc_ast_destroy_if_exists(switch_data->case_asts_v[i]);
  vector_destroy(switch_data->case_asts_v);
  free(switch_data);
}

void solc_ast_stmt_switch_add_case(solc_ast_t *switch_ast, solc_ast_t *case_ast)
{
  SOLC_ASSUME(switch_ast != nullptr &&
              switch_ast->type == SOLC_AST_TYPE_STMT_SWITCH);
  SOLC_AST_CAST(switch_data, switch_ast, ast_switch_stmt_t);
  SOLC_ASSUME(switch_data->case_asts_v != nullptr);
  vector_push(switch_data->case_asts_v, case_ast);
}

string_t *solc_ast_stmt_switch_build_tree(solc_ast_t *switch_ast)
{
  SOLC_ASSUME(switch_ast != nullptr &&
              switch_ast->type == SOLC_AST_TYPE_STMT_SWITCH);
  SOLC_AST_CAST(switch_data, switch_ast, ast_switch_stmt_t);
  SOLC_ASSUME(switch_data->case_asts_v != nullptr);
  string_t header = string_create_from("STMT_SWITCH");
  sz case_asts_v_size = vector_get_length(switch_data->case_asts_v);
  string_t **children_vs_v = vector_reserve(string_t *, case_asts_v_size + 1);
  solc_ast_add_to_tree_if_exists(children_vs_v, switch_data->expr_ast);
  for (sz i = 0; i < case_asts_v_size; i++)
    solc_ast_add_to_tree_if_exists(children_vs_v, switch_data->case_asts_v[i]);

  return ast_build_tree(&header, children_vs_v);
}
