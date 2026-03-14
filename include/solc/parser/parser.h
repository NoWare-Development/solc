#ifndef __SOLC_PARSER_H__
#define __SOLC_PARSER_H__

#include <solc/defs.h>
#include <solc/lexer/token.h>
#include <solc/parser/ast.h>

typedef enum {
  SOLC_PARSER_ERROR_TYPE_UNK,
  SOLC_PARSER_ERROR_TYPE_EXPECTED,
  SOLC_PARSER_ERROR_TYPE_UNEXPECTED,
  SOLC_PARSER_ERROR_TYPE_INVALID_EXPR,
  SOLC_PARSER_ERROR_TYPE_UNEXPECTED_WHITESPACE,
  SOLC_PARSER_ERROR_TYPE_NO_CHAR_AFTER_BACKSLASH,
  SOLC_PARSER_ERROR_TYPE_UNKNOWN_SPECIAL_CHARACTER,
  SOLC_PARSER_ERROR_TYPE_EXPR_EMPTY,
  SOLC_PARSER_ERROR_TYPE_EXPR_NO_OPERAND_AFTER_PREFIX,
  SOLC_PARSER_ERROR_TYPE_EXPR_PREFIX_AFTER_OPERAND,
  SOLC_PARSER_ERROR_TYPE_EXPR_NON_PREFIX_OPERATOR_AT_THE_BEGINNING,
  SOLC_PARSER_ERROR_TYPE_EXPR_2_NON_PREFIX_OPERATORS,
  SOLC_PARSER_ERROR_TYPE_EXPR_2_OPERANDS,
  SOLC_PARSER_ERROR_TYPE_EXPR_2_ASSIGN_OPERATORS,
  SOLC_PARSER_ERROR_TYPE_EXPR_LAST_NODE_IS_NOT_AN_OPERAND,
} solc_parser_error_type_t;

typedef struct {
  sz pos, len;
  solc_tokentype_t expected;
  solc_parser_error_type_t type : 4;
} solc_parser_error_t;

typedef struct {
  solc_token_t *tokens;
  solc_parser_error_t *errors_v;
  sz pos, tokens_num;
  b8 errored;
} solc_parser_t;

solc_parser_t solc_parser_create(solc_token_t *tokens, sz tokens_num);
void solc_parser_destroy(solc_parser_t *parser);
solc_ast_t *solc_parser_parse(solc_parser_t *parser);
solc_parser_error_t *solc_parser_get_errors(solc_parser_t *parser,
                                            sz *errors_num);

#endif // __SOLC_PARSER_H__
