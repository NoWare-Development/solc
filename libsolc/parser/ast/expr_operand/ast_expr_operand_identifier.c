#include "parser/ast_private.h"
#include "solc/parser/ast.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  SOLC_AST_HEADER;
  char *name;
} ast_expr_operand_identifier_t;

solc_ast_t *solc_ast_expr_operand_identifier_create(sz pos, const char *name)
{
  SOLC_ASSUME(name != nullptr);

  const sz name_len = strlen(name) + 1;
  ast_expr_operand_identifier_t *out_expr_operand_identifier =
    malloc(sizeof(ast_expr_operand_identifier_t) + name_len);
  SOLC_AST_INIT_HEADER(out_expr_operand_identifier, pos,
                       SOLC_AST_TYPE_EXPR_OPERAND_IDENTIFIER);
  out_expr_operand_identifier->name =
    (char *)out_expr_operand_identifier + sizeof(ast_expr_operand_identifier_t);
  memcpy(out_expr_operand_identifier->name, name, name_len);
  return SOLC_AST(out_expr_operand_identifier);
}

void solc_ast_expr_operand_identifier_destroy(solc_ast_t *id_expr_operand_ast)
{
  SOLC_ASSUME(id_expr_operand_ast != nullptr &&
              id_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_IDENTIFIER);
  free(id_expr_operand_ast);
}

sz solc_ast_expr_operand_identifier_to_string(char *buf, sz n,
                                              solc_ast_t *id_expr_operand_ast)
{
  SOLC_ASSUME(buf != nullptr && id_expr_operand_ast != nullptr &&
              id_expr_operand_ast->type ==
                SOLC_AST_TYPE_EXPR_OPERAND_IDENTIFIER);
  SOLC_AST_CAST(id_expr_operand_data, id_expr_operand_ast,
                ast_expr_operand_identifier_t);
  SOLC_ASSUME(id_expr_operand_data->name != nullptr);
  return snprintf(buf, n, "EXPR_OPERAND_IDENTIFIER { name: \"%s\" }",
                  id_expr_operand_data->name);
}
