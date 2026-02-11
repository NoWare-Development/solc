#include "parser/ast_private.h"
#include <stdlib.h>

typedef struct {
  SOLC_AST_HEADER;
  solc_ast_t *from_ast;
  solc_ast_t *what_ast;
} ast_expr_operand_access_member_t;

solc_ast_t *solc_ast_expr_operand_access_member_create(sz pos,
                                                       solc_ast_t *from_ast,
                                                       solc_ast_t *what_ast)
{
  SOLC_ASSUME(from_ast != nullptr && what_ast != nullptr);
  ast_expr_operand_access_member_t *out_expr_operand_access_member =
    malloc(sizeof(ast_expr_operand_access_member_t));
  SOLC_AST_INIT_HEADER(out_expr_operand_access_member, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_ACCESS_MEMBER);
  out_expr_operand_access_member->from_ast = from_ast;
  out_expr_operand_access_member->what_ast = what_ast;
  return SOLC_AST(out_expr_operand_access_member);
}

void solc_ast_expr_operand_access_member_destroy(
  solc_ast_t *access_member_expr_operand_ast)
{
  SOLC_ASSUME(access_member_expr_operand_ast != nullptr &&
              access_member_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_ACCESS_MEMBER);
  SOLC_AST_CAST(access_member_expr_operand_data, access_member_expr_operand_ast,
                ast_expr_operand_access_member_t);
  SOLC_ASSUME(access_member_expr_operand_data->from_ast != nullptr &&
              access_member_expr_operand_data->what_ast != nullptr);
  solc_ast_destroy(access_member_expr_operand_data->from_ast);
  solc_ast_destroy(access_member_expr_operand_data->what_ast);
  free(access_member_expr_operand_data);
}

sz solc_ast_expr_operand_access_member_to_string(
  char *buf, sz n, solc_ast_t *access_member_expr_operand_ast)
{
  SOLC_ASSUME(buf != nullptr && access_member_expr_operand_ast != nullptr &&
              access_member_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_ACCESS_MEMBER);
  SOLC_AST_CAST(access_member_expr_operand_data, access_member_expr_operand_ast,
                ast_expr_operand_access_member_t);
  SOLC_ASSUME(access_member_expr_operand_data->from_ast != nullptr &&
              access_member_expr_operand_data->what_ast != nullptr);
  SOLC_TODO("Access member expression operand to string.");
  return 0;
}
