#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
  char *name;
} ast_vardecl_t;

solc_ast_t *solc_ast_var_decl_create(sz pos, const char *name,
                                     solc_ast_t *type_ast)
{
  SOLC_ASSUME(name != nullptr && type_ast != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_vardecl_t *out_vardecl = malloc(sizeof(ast_vardecl_t) + name_len);
  SOLC_AST_INIT_HEADER(out_vardecl, pos, SOLC_AST_TYPE_VAR_DECL);
  out_vardecl->type_ast = type_ast;
  out_vardecl->name = (char *)out_vardecl + sizeof(ast_vardecl_t);
  memcpy(out_vardecl->name, name, name_len);
  return SOLC_AST(out_vardecl);
}

void solc_ast_var_decl_destroy(solc_ast_t *var_decl_ast)
{
  SOLC_ASSUME(var_decl_ast != nullptr &&
              var_decl_ast->type == SOLC_AST_TYPE_VAR_DECL);
  SOLC_AST_CAST(vardecl_data, var_decl_ast, ast_vardecl_t);
  SOLC_ASSUME(vardecl_data->type_ast != nullptr);
  solc_ast_destroy(vardecl_data->type_ast);
  free(var_decl_ast);
}

sz solc_ast_var_decl_to_string(char *buf, sz n, solc_ast_t *var_decl_ast)
{
  SOLC_ASSUME(buf != nullptr && var_decl_ast != nullptr &&
              var_decl_ast->type == SOLC_AST_TYPE_VAR_DECL);

  SOLC_TODO("Variable declaration to string.");

  return 0;
}
