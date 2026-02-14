#ifndef __SOLC_AST_GROUP_EXPR_OPERAND_H__
#define __SOLC_AST_GROUP_EXPR_OPERAND_H__

#include "containers/string.h"
#include "solc/parser/ast.h"
#include <stdint.h>

solc_ast_t *solc_ast_expr_operand_void_create(sz pos);
void solc_ast_expr_operand_void_destroy(solc_ast_t *void_expr_operand_ast);
string_t *
solc_ast_expr_operand_void_build_tree(solc_ast_t *void_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_identifier_create(sz pos, const char *name);
void solc_ast_expr_operand_identifier_destroy(solc_ast_t *id_expr_operand_ast);
string_t *
solc_ast_expr_operand_identifier_build_tree(solc_ast_t *id_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_num_create(sz pos, int64_t value,
                                             const char *typespec);
void solc_ast_expr_operand_num_destroy(solc_ast_t *num_expr_operand_ast);
string_t *
solc_ast_expr_operand_num_build_tree(solc_ast_t *num_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_numfloat_create(sz pos, double value,
                                                  const char *typespec);
void solc_ast_expr_operand_numfloat_destroy(
  solc_ast_t *numfloat_expr_operand_ast);
string_t *solc_ast_expr_operand_numfloat_build_tree(
  solc_ast_t *numfloat_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_array_element_create(sz pos,
                                                       solc_ast_t *expr_ast,
                                                       solc_ast_t *parent_ast);
void solc_ast_expr_operand_array_element_destroy(
  solc_ast_t *array_element_expr_operand_ast);
string_t *solc_ast_expr_operand_array_element_build_tree(
  solc_ast_t *array_element_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_cast_to_create(sz pos, solc_ast_t *type_ast,
                                                 solc_ast_t *expr_ast);
void solc_ast_expr_operand_cast_to_destroy(solc_ast_t *cast_to_expr_operand_ast);
string_t *
solc_ast_expr_operand_cast_to_build_tree(solc_ast_t *cast_to_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_call_create(sz pos, const char *callee_name,
                                              solc_ast_t *call_args_ast);
void solc_ast_expr_operand_call_destroy(solc_ast_t *call_expr_operand_ast);
string_t *
solc_ast_expr_operand_call_build_tree(solc_ast_t *call_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_string_create(sz pos, const char *value);
void solc_ast_expr_operand_string_destroy(solc_ast_t *string_expr_operand_ast);
string_t *
solc_ast_expr_operand_string_build_tree(solc_ast_t *string_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_symbol_create(sz pos, char value);
void solc_ast_expr_operand_symbol_destroy(solc_ast_t *symbol_expr_operand_ast);
string_t *
solc_ast_expr_operand_symbol_build_tree(solc_ast_t *symbol_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_access_member_create(sz pos,
                                                       solc_ast_t *what_ast);
void solc_ast_expr_operand_access_member_destroy(
  solc_ast_t *access_member_expr_operand_ast);
string_t *solc_ast_expr_operand_access_member_build_tree(
  solc_ast_t *access_member_expr_operand_ast);

solc_ast_t *
solc_ast_expr_operand_generic_call_create(sz pos, const char *callee_name,
                                          solc_ast_t *call_args_ast,
                                          solc_ast_t *generic_type_list_ast);
void solc_ast_expr_operand_generic_call_destroy(
  solc_ast_t *generic_call_expr_operand_ast);
string_t *solc_ast_expr_operand_generic_call_build_tree(
  solc_ast_t *generic_call_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_sizeof_create(sz pos, solc_ast_t *type_ast);
void solc_ast_expr_operand_sizeof_destroy(solc_ast_t *sizeof_expr_operand_ast);
string_t *
solc_ast_expr_operand_sizeof_build_tree(solc_ast_t *sizeof_expr_operand_ast);

#endif // __SOLC_AST_GROUP_EXPR_OPERAND_H__
