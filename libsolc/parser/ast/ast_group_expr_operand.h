#ifndef __SOLC_AST_GROUP_EXPR_OPERAND_H__
#define __SOLC_AST_GROUP_EXPR_OPERAND_H__

#include "containers/string.h"
#include "solc/parser/ast.h"

solc_ast_t *solc_ast_expr_operand_void_create(sz pos);
void solc_ast_expr_operand_void_destroy(solc_ast_t *void_expr_operand_ast);
string_t *
solc_ast_expr_operand_void_build_tree(solc_ast_t *void_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_identifier_create(sz pos, const char *name);
void solc_ast_expr_operand_identifier_destroy(solc_ast_t *id_expr_operand_ast);
string_t *
solc_ast_expr_operand_identifier_build_tree(solc_ast_t *id_expr_operand_ast);
const char *
solc_ast_expr_operand_identifier_get_name(solc_ast_t *id_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_num_create(sz pos, u64 value,
                                             const char *typespec);
void solc_ast_expr_operand_num_destroy(solc_ast_t *num_expr_operand_ast);
string_t *
solc_ast_expr_operand_num_build_tree(solc_ast_t *num_expr_operand_ast);
u64 solc_ast_expr_operand_num_get_value(solc_ast_t *num_expr_operand_ast);
const char *
solc_ast_expr_operand_num_get_typespec(solc_ast_t *num_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_numfloat_create(sz pos, f64 value,
                                                  const char *typespec);
void solc_ast_expr_operand_numfloat_destroy(
  solc_ast_t *numfloat_expr_operand_ast);
string_t *solc_ast_expr_operand_numfloat_build_tree(
  solc_ast_t *numfloat_expr_operand_ast);
f64 solc_ast_expr_operand_numfloat_get_value(
  solc_ast_t *numfloat_expr_operand_ast);
const char *solc_ast_expr_operand_numfloat_get_typespec(
  solc_ast_t *numfloat_expr_operand_ast);

solc_ast_t *
solc_ast_expr_operand_array_element_create(sz pos, solc_ast_t *index_expr_ast,
                                           solc_ast_t *parent_ast);
void solc_ast_expr_operand_array_element_destroy(
  solc_ast_t *array_element_expr_operand_ast);
string_t *solc_ast_expr_operand_array_element_build_tree(
  solc_ast_t *array_element_expr_operand_ast);
solc_ast_t *solc_ast_expr_operand_array_element_get_index_expr_ast(
  solc_ast_t *array_element_expr_operand_ast);
solc_ast_t *solc_ast_expr_operand_array_element_get_parent_ast(
  solc_ast_t *array_element_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_cast_to_create(sz pos, solc_ast_t *type_ast,
                                                 solc_ast_t *expr_ast);
void solc_ast_expr_operand_cast_to_destroy(solc_ast_t *cast_to_expr_operand_ast);
string_t *
solc_ast_expr_operand_cast_to_build_tree(solc_ast_t *cast_to_expr_operand_ast);
solc_ast_t *solc_ast_expr_operand_cast_to_get_type_ast(
  solc_ast_t *cast_to_expr_operand_ast);
solc_ast_t *solc_ast_expr_operand_cast_to_get_expr_ast(
  solc_ast_t *cast_to_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_call_create(sz pos, const char *callee_name);
void solc_ast_expr_operand_call_destroy(solc_ast_t *call_expr_operand_ast);
void solc_ast_expr_operand_call_add_argument(solc_ast_t *call_expr_operand_ast,
                                             solc_ast_t *argument_ast);
string_t *
solc_ast_expr_operand_call_build_tree(solc_ast_t *call_expr_operand_ast);
const char *
solc_ast_expr_operand_call_get_callee_name(solc_ast_t *call_expr_operand_ast);
solc_ast_t **
solc_ast_expr_operand_call_get_argument_asts(solc_ast_t *call_expr_operand_ast,
                                             sz *out_n);

solc_ast_t *solc_ast_expr_operand_string_create(sz pos, const char *value);
void solc_ast_expr_operand_string_destroy(solc_ast_t *string_expr_operand_ast);
string_t *
solc_ast_expr_operand_string_build_tree(solc_ast_t *string_expr_operand_ast);
const char *
solc_ast_expr_operand_string_get_value(solc_ast_t *string_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_symbol_create(sz pos, char value);
void solc_ast_expr_operand_symbol_destroy(solc_ast_t *symbol_expr_operand_ast);
string_t *
solc_ast_expr_operand_symbol_build_tree(solc_ast_t *symbol_expr_operand_ast);
char solc_ast_expr_operand_symbol_get_value(solc_ast_t *symbol_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_access_member_create(sz pos,
                                                       solc_ast_t *from_ast,
                                                       solc_ast_t *what_ast);
void solc_ast_expr_operand_access_member_destroy(
  solc_ast_t *access_member_expr_operand_ast);
string_t *solc_ast_expr_operand_access_member_build_tree(
  solc_ast_t *access_member_expr_operand_ast);
solc_ast_t *solc_ast_expr_operand_access_member_get_from_ast(
  solc_ast_t *access_member_expr_operand_ast);
solc_ast_t *solc_ast_expr_operand_access_member_get_what_ast(
  solc_ast_t *access_member_expr_operand_ast);

solc_ast_t *
solc_ast_expr_operand_generic_call_create(sz pos, const char *callee_name,
                                          solc_ast_t *generic_type_list_ast);
void solc_ast_expr_operand_generic_call_destroy(
  solc_ast_t *generic_call_expr_operand_ast);
void solc_ast_expr_operand_generic_call_add_argument(
  solc_ast_t *generic_call_expr_operand_ast, solc_ast_t *argument_ast);
string_t *solc_ast_expr_operand_generic_call_build_tree(
  solc_ast_t *generic_call_expr_operand_ast);
const char *solc_ast_expr_operand_generic_call_get_callee_name(
  solc_ast_t *generic_call_expr_operand_ast);
solc_ast_t *solc_ast_expr_operand_generic_call_get_generic_type_list_ast(
  solc_ast_t *generic_call_expr_operand_ast);
solc_ast_t **solc_ast_expr_operand_generic_call_get_argument_asts(
  solc_ast_t *generic_call_expr_operand_ast, sz *out_n);

solc_ast_t *solc_ast_expr_operand_sizeof_create(sz pos, solc_ast_t *type_ast);
void solc_ast_expr_operand_sizeof_destroy(solc_ast_t *sizeof_expr_operand_ast);
string_t *
solc_ast_expr_operand_sizeof_build_tree(solc_ast_t *sizeof_expr_operand_ast);
solc_ast_t *
solc_ast_expr_operand_sizeof_get_type_ast(solc_ast_t *sizeof_expr_operand_ast);

solc_ast_t *solc_ast_expr_operand_alignof_create(sz pos, solc_ast_t *expr_ast);
void solc_ast_expr_operand_alignof_destroy(solc_ast_t *alignof_expr_operand_ast);
string_t *
solc_ast_expr_operand_alignof_build_tree(solc_ast_t *alignof_expr_operand_ast);
solc_ast_t *solc_ast_expr_operand_alignof_get_expr_ast(
  solc_ast_t *alignof_expr_operand_ast);

#endif // __SOLC_AST_GROUP_EXPR_OPERAND_H__
