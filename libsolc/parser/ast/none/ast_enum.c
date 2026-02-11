#include "containers/vector.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t **elements_v;
  char *name;
} ast_enum_t;

solc_ast_t *solc_ast_enum_create(sz pos, const char *name)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_enum_t *out_enum_ast = malloc(sizeof(ast_enum_t) + name_len);
  SOLC_AST_INIT_HEADER(out_enum_ast, pos, SOLC_AST_TYPE_NONE_ENUM);
  out_enum_ast->elements_v = vector_create(solc_ast_t *);
  out_enum_ast->name = (char *)out_enum_ast + sizeof(ast_enum_t);
  memcpy(out_enum_ast->name, name, name_len);

  return SOLC_AST(out_enum_ast);
}

void solc_ast_enum_destroy(solc_ast_t *enum_ast)
{
  SOLC_ASSUME(enum_ast != nullptr && enum_ast->type == SOLC_AST_TYPE_NONE_ENUM);

  SOLC_AST_CAST(enum_data, enum_ast, ast_enum_t);
  SOLC_ASSUME(enum_data->elements_v != nullptr);
  for (sz i = 0, elements_v_size = vector_get_length(enum_data->elements_v);
       i < elements_v_size; i++) {
    solc_ast_destroy(enum_data->elements_v[i]);
  }
  vector_destroy(enum_data->elements_v);
  free(enum_data);
}

void solc_ast_enum_add_element(solc_ast_t *enum_ast,
                               solc_ast_t *enum_element_ast)
{
  SOLC_ASSUME(enum_ast != nullptr &&
              enum_ast->type == SOLC_AST_TYPE_NONE_ENUM &&
              enum_element_ast != nullptr);

  SOLC_AST_CAST(enum_data, enum_ast, ast_enum_t);
  SOLC_ASSUME(enum_data->elements_v != nullptr);

  vector_push(enum_data->elements_v, enum_element_ast);
}

sz solc_ast_enum_to_string(char *buf, sz n, solc_ast_t *enum_ast)
{
  SOLC_ASSUME(buf != nullptr && enum_ast != nullptr &&
              enum_ast->type == SOLC_AST_TYPE_NONE_ENUM);

  SOLC_TODO("Enum AST to string.");
  return 0;
}
