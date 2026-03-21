#ifndef __SOLC_PARSER_PRIVATE_H__
#define __SOLC_PARSER_PRIVATE_H__

#include "parser/ast_func_type.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

#define VERIFY_POS(_parser, _pos)                                         \
  {                                                                       \
    if (!solc_parser_verify_pos((_parser), (_pos))) {                     \
      fprintf(stderr, "[%s]: ERROR <EXPECTED(token)> %d\n", __FUNCTION__, \
              __LINE__);                                                  \
      return nullptr;                                                     \
    }                                                                     \
  }
#define VERIFY_TOKEN(_parser, _pos, _got, _expected)                         \
  {                                                                          \
    if (!solc_parser_verify_token((_parser), (_pos), (_got), (_expected))) { \
      fprintf(stderr, "[%s]: ERROR <UNEXPECTED(token)> %d\n", __FUNCTION__,  \
              __LINE__);                                                     \
      return nullptr;                                                        \
    }                                                                        \
  }
#define VERIFY_VALUE(_parser, _pos, _got, _expected)                         \
  {                                                                          \
    if (!solc_parser_verify_value((_parser), (_pos), (_got), (_expected))) { \
      fprintf(stderr, "[%s]: ERROR <UNEXPECTED(value)> %d\n", __FUNCTION__,  \
              __LINE__);                                                     \
      return nullptr;                                                        \
    }                                                                        \
  }
#define VERIFY_WHITESPACE(_parser, _pos, _got, _expected, _next_tok)           \
  {                                                                            \
    if (!solc_parser_verify_whitespace((_parser), (_pos), (_got), (_expected), \
                                       (_next_tok))) {                         \
      fprintf(stderr, "[%s]: ERROR <UNEXPECTED(whitespace)> %d\n",             \
              __FUNCTION__, __LINE__);                                         \
      return nullptr;                                                          \
    }                                                                          \
  }

solc_ast_t *solc_parser_parse_top(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_typedef(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_type(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_type_raw(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_type_funcptr(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_enum(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_enum_element(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_import(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_module(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_export(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_extern(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_decldef(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_decldef_var(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_def_func(solc_parser_t *parser,
                                       solc_ast_func_type_t func_type);
solc_ast_t *solc_parser_parse_def_func_generic(solc_parser_t *parser,
                                               solc_ast_func_type_t func_type);
solc_ast_t *solc_parser_parse_func_arg(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_expr(solc_parser_t *parser);
solc_ast_t *
solc_parser_parse_stmt_expr_or_generic_func_or_namespace(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_block(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_return(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_goto(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_label(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_while(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_for(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_dowhile(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_switch(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_case(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_default(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_defer(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_if(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_else(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_break(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_continue(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_stmt_fallthrough(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_struct(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_union(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_initlist(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_initlist_entry(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_func_arglist(solc_parser_t *parser);
solc_ast_t *
solc_parser_parse_generic_placeholder_type_list(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_generic_type_list(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_expr(solc_parser_t *parser, b8 top_level);
solc_ast_t *solc_parser_parse_expr_operand(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_expr_operand_call(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_expr_operand_generic_call(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_generic_namespace(solc_parser_t *parser);
b8 solc_parser_is_expr_operand_generic_call(solc_parser_t *parser);
b8 solc_parser_is_generic_namespace(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_expr_operand_identifier(solc_parser_t *parser,
                                                      b8 accept_namespaces,
                                                      b8 accept_functions);
solc_ast_t *solc_parser_parse_expr_operand_array_element(solc_parser_t *parser,
                                                         solc_ast_t *array_ast);
solc_ast_t *solc_parser_parse_vismarker(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_attribute(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_attribute_list(solc_parser_t *parser);
solc_ast_t *solc_parser_parse_attribute_list_optional(solc_parser_t *parser);

void solc_parser_skip_until(solc_parser_t *parser, solc_tokentype_t token_type);
void solc_parser_advance_to_terminator(solc_parser_t *parser);

b8 solc_parser_verify_pos(solc_parser_t *parser, sz pos);
b8 solc_parser_verify_token(solc_parser_t *parser, sz pos, solc_tokentype_t got,
                            solc_tokentype_t expected);
b8 solc_parser_verify_value(solc_parser_t *parser, sz pos, const char *got,
                            const char *expected);
b8 solc_parser_verify_whitespace(solc_parser_t *parser, sz pos, b8 got,
                                 b8 expected,
                                 solc_tokentype_t expected_tokentype_after);

solc_tokentype_t solc_parser_peek(solc_parser_t *parser, sz pos);
const solc_token_t *solc_parser_peek_token(solc_parser_t *parser, sz pos);

void solc_parser_add_error(solc_parser_t *parser, solc_parser_error_type_t type,
                           sz pos, sz len, solc_tokentype_t expected);

b8 solc_parser_is_qualifier(const char *str);

b8 solc_parser_is_operator_token(solc_tokentype_t type);
b8 solc_parser_is_binary_operator_token(solc_tokentype_t type);
b8 solc_parser_is_assign_operator_token(solc_tokentype_t type);
b8 solc_parser_is_compare_operator_token(solc_tokentype_t type);
b8 solc_parser_is_prefix_operator_token(solc_tokentype_t type);
b8 solc_parser_is_numeric_token(solc_tokentype_t type);

b8 solc_parser_is_operand_ast(solc_ast_type_t type);

#endif // __SOLC_PARSER_PRIVATE_H__
