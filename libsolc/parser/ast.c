#include "solc/parser/ast.h"
#include "containers/string.h"
#include "containers/vector.h"
#include "parser/ast_op_types.h"
#include "solc/defs.h"

#include "parser/ast_private.h"

solc_ast_group_t solc_ast_type_get_group(solc_ast_type_t type)
{
  return (solc_ast_group_t)((type >> 8) & 0xFF);
}

u8 solc_ast_type_get_id_in_group(solc_ast_type_t type)
{
  return (u8)(type & 0xFF);
}

solc_ast_destroy_func_t solc_ast_get_destroy_func(solc_ast_type_t ast_type)
{
#define __SOLC_AST_TYPE_X(type_name, group_name, in_group_id, in_code_name) \
  case __SOLC_FULL_AST_NAME(type_name, group_name):                         \
    return solc_ast_##in_code_name##_destroy;

  switch (ast_type) {
    __SOLC_AST_TYPES
  default:
    SOLC_NOREACH();
  }
#undef __SOLC_AST_TYPE_X
}

void solc_ast_print(solc_ast_t *ast)
{
  SOLC_ASSUME(ast != nullptr);

  solc_ast_build_tree_func_t build_tree_func =
    ast_get_build_tree_func(ast->type);
  string_t *strs_v = build_tree_func(ast);
  sz strs_v_size = vector_get_length(strs_v);
  for (sz i = 0; i < strs_v_size; i++) {
    printf("%s\n", strs_v[i].data);
    string_destroy(&strs_v[i]);
  }
  vector_destroy(strs_v);
}

solc_ast_build_tree_func_t ast_get_build_tree_func(solc_ast_type_t ast_type)
{
#define __SOLC_AST_TYPE_X(type_name, group_name, in_group_id, in_code_name) \
  case __SOLC_FULL_AST_NAME(type_name, group_name):                         \
    return solc_ast_##in_code_name##_build_tree;

  switch (ast_type) {
    __SOLC_AST_TYPES
  default:
    SOLC_NOREACH();
  }
#undef __SOLC_AST_TYPE_X
}

string_t *ast_build_tree(string_t *heading, string_t **children_vs_v)
{
  string_t *out_v = vector_create(string_t);
  vector_push(out_v, *heading);

  for (sz i = 0, children_vs_v_size = vector_get_length(children_vs_v);
       i < children_vs_v_size; i++) {
    string_t *children_v = children_vs_v[i];
    for (sz j = 0, children_v_size = vector_get_length(children_v);
         j < children_v_size; j++) {
      string_t str;

      if (j == 0) {
        if (i == children_vs_v_size - 1) {
          str = string_create_from("╰─");
        } else {
          str = string_create_from("├─");
        }
      } else if (i < children_vs_v_size - 1) {
        str = string_create_from("│ ");
      } else {
        str = string_create_from("  ");
      }

      string_append(&str, &children_v[j]);
      string_destroy(&children_v[j]);
      vector_push(out_v, str);
    }
    vector_destroy(children_v);
  }
  vector_destroy(children_vs_v);

  return out_v;
}

expr_operator_group_t expr_operator_type_get_group(expr_operator_type_t type)
{
  return (expr_operator_group_t)((type >> 8) & 0xFF);
}

const char *ast_expr_operator_type_to_string(expr_operator_type_t t)
{
#define __EXPR_OPERATORS_X(type_name, group_name, id, display_val) \
  case __EXPR_OPERATOR_TYPE_FULL_NAME(type_name, group_name):      \
    return display_val;

  switch (t) {
    __EXPR_OPERATORS
  default:
    SOLC_NOREACH();
  }
#undef __EXPR_OPERATORS_X
}

const char *solc_ast_func_type_to_string(solc_ast_func_type_t func_type)
{
  static const char *strs[] = {
    "DEFAULT",
    "EXPLICIT",
    "EXPORTED",
  };
  return strs[func_type];
}
