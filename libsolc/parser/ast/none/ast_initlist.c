#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **init_elements_v;
} ast_initlist_t;

solc_ast_t *solc_ast_initlist_create(sz pos)
{
  ast_initlist_t *out_initlist = malloc(sizeof(ast_initlist_t));
  SOLC_AST_INIT_HEADER(out_initlist, pos, SOLC_AST_TYPE_NONE_INITLIST);
  out_initlist->init_elements_v = vector_create(solc_ast_t *);
  return SOLC_AST(out_initlist);
}

void solc_ast_initlist_destroy(solc_ast_t *initlist_ast)
{
  SOLC_ASSUME(initlist_ast != nullptr &&
              initlist_ast->type == SOLC_AST_TYPE_NONE_INITLIST);
  SOLC_AST_CAST(initlist_data, initlist_ast, ast_initlist_t);
  SOLC_ASSUME(initlist_data->init_elements_v != nullptr);
  for (sz i = 0, init_elements_v_len =
                   vector_get_length(initlist_data->init_elements_v);
       i < init_elements_v_len; i++) {
    solc_ast_destroy(initlist_data->init_elements_v[i]);
  }
  vector_destroy(initlist_data->init_elements_v);
  free(initlist_ast);
}

void solc_ast_initlist_add_element(solc_ast_t *initlist_ast,
                                   solc_ast_t *initlist_element_ast)
{
  SOLC_ASSUME(initlist_ast != nullptr &&
              initlist_ast->type == SOLC_AST_TYPE_NONE_INITLIST &&
              initlist_element_ast != nullptr);
  SOLC_AST_CAST(initlist_data, initlist_ast, ast_initlist_t);
  SOLC_ASSUME(initlist_data->init_elements_v != nullptr);
  vector_push(initlist_data->init_elements_v, initlist_element_ast);
}

sz solc_ast_initlist_to_string(char *buf, sz n, solc_ast_t *initlist_ast)
{
  SOLC_ASSUME(buf != nullptr && initlist_ast != nullptr &&
              initlist_ast->type == SOLC_AST_TYPE_NONE_INITLIST);

  SOLC_TODO("Initialization list to string.");

  return 0;
}
