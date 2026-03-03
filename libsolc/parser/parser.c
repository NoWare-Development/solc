#include "solc/parser/parser.h"
#include "containers/vector.h"
#include "parser/ast/ast_group_none.h"
#include "parser/parser_context.h"
#include "solc/defs.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include <string.h>

solc_parser_t solc_parser_create(solc_token_t *tokens, sz tokens_num)
{
  return (solc_parser_t){
    .tokens = tokens,
    .errors_v = vector_create(solc_parser_error_t),
    .pos = 0,
    .tokens_num = tokens_num,
    .errored = false,
  };
}

void solc_parser_destroy(solc_parser_t *parser)
{
  SOLC_ASSUME(parser != nullptr);
  vector_destroy(parser->errors_v);
  memset(parser, 0, sizeof(solc_parser_t));
}

solc_ast_t *solc_parser_parse(solc_parser_t *parser)
{
  SOLC_ASSUME(parser != nullptr);
  solc_ast_t *root = solc_ast_root_create();
  while (parser->pos < parser->tokens_num) {
    solc_ast_t *top = solc_parser_parse_top(parser);
    if (parser->errored) {
      parser->errored = false;
      continue;
    }
    solc_ast_root_add_top_statement(root, top);
  }

  return root;
}

solc_parser_error_t *solc_parser_get_errors(solc_parser_t *parser,
                                            sz *errors_num)
{
  SOLC_ASSUME(parser != nullptr && errors_num != nullptr);
  *errors_num = vector_get_length(parser->errors_v);
  return parser->errors_v;
}

void solc_parser_skip_until(solc_parser_t *parser, solc_tokentype_t token_type)
{
  while (parser->pos < parser->tokens_num) {
    if (parser->tokens[parser->pos].type == token_type)
      return;
    parser->pos++;
  }
}

void solc_parser_advance_to_terminator(solc_parser_t *parser)
{
  while (parser->pos < parser->tokens_num) {
    solc_tokentype_t type = parser->tokens[parser->pos].type;
    switch (type) {
    case SOLC_TOKENTYPE_LPAREN:
    case SOLC_TOKENTYPE_RPAREN:
    case SOLC_TOKENTYPE_LCBRACK:
    case SOLC_TOKENTYPE_RCBRACK:
    case SOLC_TOKENTYPE_SEMI:
    case SOLC_TOKENTYPE_COMMA:
      return;

    default:
      break;
    }
    parser->pos++;
  }
}

b8 solc_parser_verify_pos(solc_parser_t *parser, sz pos)
{
  if (pos >= parser->tokens_num) {
    solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_EXPECTED, pos, 1,
                          SOLC_TOKENTYPE_ERR);
    return false;
  }
  return true;
}

b8 solc_parser_verify_token(solc_parser_t *parser, sz pos, solc_tokentype_t got,
                            solc_tokentype_t expected)
{
  if (got != expected) {
    solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_UNEXPECTED, pos, 1,
                          SOLC_TOKENTYPE_ERR);
    return false;
  }
  return true;
}

b8 solc_parser_verify_value(solc_parser_t *parser, sz pos, const char *got,
                            const char *expected)
{
  if (strcmp(got, expected) != 0) {
    solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_EXPECTED, pos, 1,
                          SOLC_TOKENTYPE_ERR);
    return false;
  }
  return true;
}

b8 solc_parser_verify_whitespace(solc_parser_t *parser, sz pos, b8 got,
                                 b8 expected,
                                 solc_tokentype_t expected_tokentype_after)
{
  if (got != expected) {
    solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_UNEXPECTED_WHITESPACE,
                          pos, 1, expected_tokentype_after);
    return false;
  }
  return true;
}

solc_tokentype_t solc_parser_peek(solc_parser_t *parser, sz pos)
{
  if (pos >= parser->tokens_num)
    return SOLC_TOKENTYPE_ERR;
  return parser->tokens[pos].type;
}

const solc_token_t *solc_parser_peek_token(solc_parser_t *parser, sz pos)
{
  if (pos >= parser->tokens_num)
    return nullptr;
  return &parser->tokens[pos];
}

void solc_parser_add_error(solc_parser_t *parser, solc_parser_error_type_t type,
                           sz pos, sz len, solc_tokentype_t expected_after)
{
  parser->errored = true;
  solc_parser_error_t error = {
    .len = len,
    .pos = pos,
    .expected_after = expected_after,
    .type = type,
  };
  vector_push(parser->errors_v, error);
  solc_parser_advance_to_terminator(parser);
}

b8 solc_parser_is_qualifier(const char *str)
{
  return parser_context_is_qualifier(str);
}

b8 solc_parser_is_operator_token(solc_tokentype_t type)
{
  return solc_parser_is_binary_operator_token(type) ||
         solc_parser_is_assign_operator_token(type) ||
         solc_parser_is_compare_operator_token(type) ||
         solc_parser_is_prefix_operator_token(type);
}

b8 solc_parser_is_binary_operator_token(solc_tokentype_t type)
{
  switch (type) {
  case SOLC_TOKENTYPE_PLUS:
  case SOLC_TOKENTYPE_MINUS:
  case SOLC_TOKENTYPE_ASTERISK:
  case SOLC_TOKENTYPE_SLASH:
  case SOLC_TOKENTYPE_PERCENT:
  case SOLC_TOKENTYPE_AMPERSAND:
  case SOLC_TOKENTYPE_PIPE:
  case SOLC_TOKENTYPE_CIRCUMFLEX:
    return true;
  default:
    return false;
  }
}

b8 solc_parser_is_assign_operator_token(solc_tokentype_t type)
{
  return type == SOLC_TOKENTYPE_EQ;
}

b8 solc_parser_is_compare_operator_token(solc_tokentype_t type)
{
  return type == SOLC_TOKENTYPE_RARROW || type == SOLC_TOKENTYPE_LARROW;
}

b8 solc_parser_is_prefix_operator_token(solc_tokentype_t type)
{
  switch (type) {
  case SOLC_TOKENTYPE_EXCLMARK:
  case SOLC_TOKENTYPE_TILDE:
  case SOLC_TOKENTYPE_MINUS:
  case SOLC_TOKENTYPE_ASTERISK:
  case SOLC_TOKENTYPE_AMPERSAND:
    return true;
  default:
    return false;
  }
}

b8 solc_parser_is_numeric_token(solc_tokentype_t type)
{
  switch (type) {
  case SOLC_TOKENTYPE_NUM:
  case SOLC_TOKENTYPE_NUMHEX:
  case SOLC_TOKENTYPE_NUMBIN:
  case SOLC_TOKENTYPE_NUMOCT:
  case SOLC_TOKENTYPE_NUMFLOAT:
    return true;
  default:
    return false;
  }
}

b8 solc_parser_is_operand_ast(solc_ast_type_t type)
{
  switch (type) {
  case SOLC_AST_TYPE_NONE_EXPR:
  case SOLC_AST_TYPE_NONE_PREFIX_EXPR:
  case SOLC_AST_TYPE_NONE_NAMESPACE:
  case SOLC_AST_TYPE_GENERIC_NAMESPACE:
    return true;

  default: {
    solc_ast_group_t group = solc_ast_type_get_group(type);
    return group == SOLC_AST_GROUP_EXPR_OPERAND;
  }
  }
}
