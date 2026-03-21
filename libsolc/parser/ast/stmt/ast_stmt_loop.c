#include "containers/string.h"
#include "containers/vector.h"
#include "solc/parser/ast.h"
#include "parser/ast_private.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *attribute_list_ast;
  solc_ast_t *stmt_ast;
} ast_loop_t;

solc_ast_t *solc_ast_stmt_loop_create(sz pos, solc_ast_t *stmt_ast,
                                      solc_ast_t *attribute_list_ast)
{
  ast_loop_t *out_loop = malloc(sizeof(ast_loop_t));
  SOLC_AST_INIT_HEADER(out_loop, pos, SOLC_AST_TYPE_STMT_LOOP);
  out_loop->attribute_list_ast = attribute_list_ast;
  out_loop->stmt_ast = stmt_ast;
  return SOLC_AST(out_loop);
}

void solc_ast_stmt_loop_destroy(solc_ast_t *loop_ast)
{
  SOLC_ASSUME(loop_ast != nullptr && loop_ast->type == SOLC_AST_TYPE_STMT_LOOP);
  SOLC_AST_CAST(loop_data, loop_ast, ast_loop_t);
  solc_ast_destroy_if_exists(loop_data->attribute_list_ast);
  solc_ast_destroy_if_exists(loop_data->stmt_ast);
  free(loop_data);
}

string_t *solc_ast_stmt_loop_build_tree(solc_ast_t *loop_ast)
{
  SOLC_ASSUME(loop_ast != nullptr && loop_ast->type == SOLC_AST_TYPE_STMT_LOOP);
  SOLC_AST_CAST(loop_data, loop_ast, ast_loop_t);
  string_t header = string_create_from("STMT_LOOP");
  string_t **children_vs_v = vector_reserve(string_t *, 2);
  solc_ast_add_to_tree_if_exists(children_vs_v, loop_data->attribute_list_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, loop_data->stmt_ast);
  return ast_build_tree(&header, children_vs_v);
}

solc_ast_t *solc_ast_stmt_loop_get_attribute_list_ast(solc_ast_t *loop_ast)
{
  SOLC_ASSUME(loop_ast != nullptr && loop_ast->type == SOLC_AST_TYPE_STMT_LOOP);
  SOLC_AST_CAST(loop_data, loop_ast, ast_loop_t);
  return loop_data->attribute_list_ast;
}

solc_ast_t *solc_ast_stmt_loop_get_stmt_ast(solc_ast_t *loop_ast)
{
  SOLC_ASSUME(loop_ast != nullptr && loop_ast->type == SOLC_AST_TYPE_STMT_LOOP);
  SOLC_AST_CAST(loop_data, loop_ast, ast_loop_t);
  return loop_data->stmt_ast;
}
