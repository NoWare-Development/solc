#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
  solc_ast_t *expr_ast;
  char *name;
} ast_vardef_t;

solc_ast_t *solc_ast_var_def_create(sz pos, const char *name,
                                    solc_ast_t *type_ast, solc_ast_t *expr_ast)
{
  SOLC_ASSUME(name != nullptr && expr_ast != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_vardef_t *out_vardef = malloc(sizeof(ast_vardef_t) + name_len);
  SOLC_AST_INIT_HEADER(out_vardef, pos, SOLC_AST_TYPE_VAR_DEF);
  out_vardef->type_ast = type_ast;
  out_vardef->expr_ast = expr_ast;
  out_vardef->name = (char *)out_vardef + sizeof(ast_vardef_t);
  memcpy(out_vardef->name, name, name_len);
  return SOLC_AST(out_vardef);
}

void solc_ast_var_def_destroy(solc_ast_t *var_def_ast)
{
  SOLC_ASSUME(var_def_ast != nullptr &&
              var_def_ast->type == SOLC_AST_TYPE_VAR_DEF);
  SOLC_AST_CAST(vardef_data, var_def_ast, ast_vardef_t);
  SOLC_ASSUME(vardef_data->expr_ast != nullptr);
  if (vardef_data->type_ast != nullptr) {
    solc_ast_destroy(vardef_data->type_ast);
  }
  solc_ast_destroy(vardef_data->expr_ast);
  free(vardef_data);
}

sz solc_ast_var_def_to_string(char *buf, sz n, solc_ast_t *var_def_ast)
{
  SOLC_ASSUME(buf != nullptr && var_def_ast != nullptr &&
              var_def_ast->type == SOLC_AST_TYPE_VAR_DEF);

  SOLC_TODO("Variable definition to string.");

  return 0;
}
