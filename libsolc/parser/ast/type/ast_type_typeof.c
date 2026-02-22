#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *expr_ast;
} ast_typeof_type_t;

solc_ast_t *solc_ast_type_typeof_create(sz pos, solc_ast_t *expr_ast)
{
  ast_typeof_type_t *out_typeof_type = malloc(sizeof(ast_typeof_type_t));
  SOLC_AST_INIT_HEADER(out_typeof_type, pos, SOLC_AST_TYPE_TYPE_TYPEOF);
  out_typeof_type->expr_ast = expr_ast;
  return SOLC_AST(out_typeof_type);
}

void solc_ast_type_typeof_destroy(solc_ast_t *typeof_type_ast)
{
  SOLC_ASSUME(typeof_type_ast != nullptr &&
              typeof_type_ast->type == SOLC_AST_TYPE_TYPE_TYPEOF);
  SOLC_AST_CAST(typeof_type_data, typeof_type_ast, ast_typeof_type_t);
  solc_ast_destroy_if_exists(typeof_type_data->expr_ast);
  free(typeof_type_ast);
}

string_t *solc_ast_type_typeof_build_tree(solc_ast_t *typeof_type_ast)
{
  SOLC_ASSUME(typeof_type_ast != nullptr &&
              typeof_type_ast->type == SOLC_AST_TYPE_TYPE_TYPEOF);
  SOLC_AST_CAST(typeof_type_data, typeof_type_ast, ast_typeof_type_t);
  string_t **children_vs_v = vector_create(string_t *);
  solc_ast_add_to_tree_if_exists(children_vs_v, typeof_type_data->expr_ast);
  string_t header = string_create_from("TYPE_TYPEOF");
  return ast_build_tree(&header, children_vs_v);
}
