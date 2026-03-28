#include "containers/vector.h"
#include "parser/ast/ast_group_generic.h"
#include "parser/parser_private.h"
#include "solc/defs.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *
solc_parser_parse_generic_placeholder_type_list(solc_parser_t *parser)
{
  solc_ast_t *generic_placeholder_type_list_ast =
    solc_ast_generic_placeholder_type_list_create(parser->pos++);

  VERIFY_POS(parser, parser->pos);
  while (parser->pos < parser->tokens_num) {
    solc_token_t cur = parser->tokens[parser->pos];
    if (cur.type == SOLC_TOKENTYPE_RARROW)
      break;

    VERIFY_TOKEN(parser, parser->pos, cur.type, SOLC_TOKENTYPE_ID);

    sz generic_placeholder_type_pos = parser->pos;
    const char *generic_placeholder_type_name = cur.value;
    solc_ast_t *generic_placeholder_type_default_type = nullptr;

    parser->pos++;
    VERIFY_POS(parser, parser->pos);
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_EQ) {
      parser->pos++;
      VERIFY_POS(parser, parser->pos);

      generic_placeholder_type_default_type = solc_parser_parse_type(parser);
    }

    solc_ast_t *generic_placeholder_type_ast =
      solc_ast_generic_placeholder_type_create(
        generic_placeholder_type_pos, generic_placeholder_type_name,
        generic_placeholder_type_default_type);
    solc_ast_generic_placeholder_type_list_add_placeholder_type(
      generic_placeholder_type_list_ast, generic_placeholder_type_ast);
    solc_tokentype_t next = solc_parser_peek(parser, parser->pos);
    if (next != SOLC_TOKENTYPE_RARROW) {
      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, next, SOLC_TOKENTYPE_COMMA);
      parser->pos++;
    }
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RARROW);
  parser->pos++;

  return generic_placeholder_type_list_ast;
}

solc_ast_t *solc_parser_parse_generic_type_list(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LARROW);

  solc_ast_t *generic_type_list =
    solc_ast_generic_type_list_create(parser->pos++);

  while (parser->pos < parser->tokens_num) {
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RARROW)
      break;

    solc_ast_t *type = solc_parser_parse_type(parser);
    solc_ast_generic_type_list_add_type(generic_type_list, type);

    VERIFY_POS(parser, parser->pos);
    if (parser->tokens[parser->pos].type != SOLC_TOKENTYPE_RARROW) {
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_COMMA);

      parser->pos++;
      VERIFY_POS(parser, parser->pos);
      if SOLC_UNLIKELY (parser->tokens[parser->pos].type ==
                        SOLC_TOKENTYPE_RARROW) {
        solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_UNEXPECTED,
                              parser->pos, 1, SOLC_TOKENTYPE_ERR);
        return nullptr;
      }
    }
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RARROW);

  parser->pos++;
  return generic_type_list;
}

b8 solc_parser_is_expr_operand_generic_call(solc_parser_t *parser)
{
  if (solc_parser_peek(parser, parser->pos + 1) != SOLC_TOKENTYPE_LARROW ||
      parser->pos + 2 >= parser->tokens_num)
    return false;

  sz old_pos = parser->pos;
  b8 maybe_generic = false;
  b8 errored_prev = parser->errored;
  sz old_errors_size = vector_get_length(parser->errors_v);
  solc_parser_error_t *old_errors = vector_copy(parser->errors_v);

  parser->pos += 2;
  while (parser->pos < parser->tokens_num) {
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RARROW) {
      maybe_generic = true;
      parser->pos++;
      break;
    }

    solc_ast_t *type_ast = solc_parser_parse_type(parser);
    if (type_ast != nullptr)
      solc_ast_destroy(type_ast);

    if (vector_get_length(parser->errors_v) != old_errors_size)
      break;

    solc_tokentype_t next = solc_parser_peek(parser, parser->pos);
    if (next != SOLC_TOKENTYPE_RARROW) {
      if (next != SOLC_TOKENTYPE_COMMA)
        break;

      parser->pos++;
    }
  }

  b8 result = maybe_generic &&
              solc_parser_peek(parser, parser->pos) == SOLC_TOKENTYPE_LPAREN;

  vector_destroy(parser->errors_v);
  parser->errors_v = old_errors;
  parser->errored = errored_prev;
  parser->pos = old_pos;

  return result;
}

b8 solc_parser_is_generic_namespace(solc_parser_t *parser)
{
  if (solc_parser_peek(parser, parser->pos + 1) != SOLC_TOKENTYPE_LARROW ||
      parser->pos + 2 >= parser->tokens_num)
    return false;

  sz old_pos = parser->pos;
  b8 maybe_generic = false;
  b8 errored_prev = parser->errored;
  sz old_errors_size = vector_get_length(parser->errors_v);
  solc_parser_error_t *old_errors = vector_copy(parser->errors_v);

  parser->pos += 2;

  while (parser->pos < parser->tokens_num) {
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RARROW) {
      maybe_generic = true;
      parser->pos++;
      break;
    }

    solc_ast_t *type_ast = solc_parser_parse_type(parser);
    if (type_ast != nullptr)
      solc_ast_destroy(type_ast);

    if (vector_get_length(parser->errors_v) != old_errors_size)
      break;

    solc_tokentype_t next = solc_parser_peek(parser, parser->pos);
    if (next != SOLC_TOKENTYPE_RARROW) {
      if (next != SOLC_TOKENTYPE_COMMA)
        break;

      parser->pos++;
    }
  }

  const solc_token_t *peeked = solc_parser_peek_token(parser, parser->pos);
  b8 result =
    maybe_generic && peeked != nullptr &&
    peeked->type == SOLC_TOKENTYPE_COLON && !peeked->has_whitespace_after &&
    solc_parser_peek(parser, parser->pos + 1) == SOLC_TOKENTYPE_COLON &&
    solc_parser_peek(parser, parser->pos + 2) == SOLC_TOKENTYPE_ID;

  vector_destroy(parser->errors_v);
  parser->errors_v = old_errors;
  parser->errored = errored_prev;
  parser->pos = old_pos;

  return result;
}

solc_ast_t *solc_parser_parse_generic_namespace(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);

  const char *namespace_name = parser->tokens[parser->pos].value;
  sz namespace_pos = parser->pos;

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LARROW);

  solc_ast_t *generic_type_list = solc_parser_parse_generic_type_list(parser);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_COLON);
  VERIFY_WHITESPACE(parser, parser->pos,
                    parser->tokens[parser->pos].has_whitespace_after, false,
                    SOLC_TOKENTYPE_COLON);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_COLON);

  parser->pos++;

  return solc_ast_generic_namespace_create(namespace_pos, namespace_name,
                                           generic_type_list, nullptr);
}
