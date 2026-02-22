#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast_private.h"
#include "solc/defs.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *type_ast;
  solc_ast_t *arg_list_ast;
  char *name;
} ast_extern_func_t;

solc_ast_t *solc_ast_extern_func_create(sz pos, const char *name,
                                        solc_ast_t *type_ast,
                                        solc_ast_t *arg_list_ast)
{
  SOLC_ASSUME(name != nullptr);
  const sz name_len = strlen(name) + 1;
  ast_extern_func_t *out_extern_func =
    malloc(sizeof(ast_extern_func_t) + name_len);
  SOLC_AST_INIT_HEADER(out_extern_func, pos, SOLC_AST_TYPE_NONE_EXTERN_FUNC);
  out_extern_func->type_ast = type_ast;
  out_extern_func->arg_list_ast = arg_list_ast;
  out_extern_func->name = (char *)out_extern_func + sizeof(ast_extern_func_t);
  memcpy(out_extern_func->name, name, name_len);
  return SOLC_AST(out_extern_func);
}

void solc_ast_extern_func_destroy(solc_ast_t *extern_func_ast)
{
  SOLC_ASSUME(extern_func_ast != nullptr &&
              extern_func_ast->type == SOLC_AST_TYPE_NONE_EXTERN_FUNC);
  SOLC_AST_CAST(extern_func_data, extern_func_ast, ast_extern_func_t);
  solc_ast_destroy_if_exists(extern_func_data->type_ast);
  solc_ast_destroy_if_exists(extern_func_data->arg_list_ast);
  free(extern_func_data);
}

string_t *solc_ast_extern_func_build_tree(solc_ast_t *extern_func_ast)
{
  SOLC_ASSUME(extern_func_ast != nullptr &&
              extern_func_ast->type == SOLC_AST_TYPE_NONE_EXTERN_FUNC);
  SOLC_AST_CAST(extern_func_data, extern_func_ast, ast_extern_func_t);
  SOLC_ASSUME(extern_func_data->name != nullptr);

  string_t header = string_create_from("EXTERN_FUNC { name: \"");
  string_append_cstr(&header, extern_func_data->name);
  string_append_cstr(&header, "\" }");

  string_t **children_vs_v = vector_reserve(string_t *, 2);
  solc_ast_add_to_tree_if_exists(children_vs_v, extern_func_data->type_ast);
  solc_ast_add_to_tree_if_exists(children_vs_v, extern_func_data->arg_list_ast);

  return ast_build_tree(&header, children_vs_v);
}
