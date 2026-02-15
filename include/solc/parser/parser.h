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
} solc_parser_error_type_t;

typedef struct {
  sz pos, len;
  solc_parser_error_type_t type : 3;
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
