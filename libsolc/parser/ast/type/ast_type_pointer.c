#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
} ast_pointer_type_t;

solc_ast_t *solc_ast_type_pointer_create(sz pos, solc_ast_t *type_ast)
{
  SOLC_ASSUME(type_ast != nullptr);

  ast_pointer_type_t *out_pointer_type = malloc(sizeof(ast_pointer_type_t));
  SOLC_AST_INIT_HEADER(out_pointer_type, pos, SOLC_AST_TYPE_TYPE_POINTER);
  out_pointer_type->type_ast = type_ast;
  return SOLC_AST(out_pointer_type);
}

void solc_ast_type_pointer_destroy(solc_ast_t *pointer_type_ast)
{
  SOLC_ASSUME(pointer_type_ast != nullptr &&
              pointer_type_ast->type == SOLC_AST_TYPE_TYPE_POINTER);
  SOLC_AST_CAST(pointer_type_data, pointer_type_ast, ast_pointer_type_t);
  SOLC_ASSUME(pointer_type_data->type_ast != nullptr);
  solc_ast_destroy(pointer_type_data->type_ast);
  free(pointer_type_data);
}

sz solc_ast_type_pointer_to_string(char *buf, sz n,
                                   solc_ast_t *pointer_type_ast)
{
  SOLC_ASSUME(buf != nullptr && pointer_type_ast != nullptr &&
              pointer_type_ast->type == SOLC_AST_TYPE_TYPE_POINTER);

  SOLC_TODO("Type pointer to string.");

  return 0;
}
