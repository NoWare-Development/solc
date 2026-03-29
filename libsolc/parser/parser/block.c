#include "containers/vector.h"
#include "parser/ast/ast_group_generic.h"
#include "parser/ast/ast_group_none.h"
#include "parser/ast/ast_group_stmt.h"
#include "parser/ast_func_type.h"
#include "parser/parser_context.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_stmt_block(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LCBRACK);

  solc_ast_t *block = solc_ast_stmt_block_create(parser->pos++);
  VERIFY_POS(parser, parser->pos);

  while (parser->pos < parser->tokens_num) {
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RCBRACK)
      break;

    solc_ast_stmt_block_add_stmt(block, solc_parser_parse_stmt(parser));
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RCBRACK);

  parser->pos++;

  return block;
}

solc_ast_t *solc_parser_parse_stmt(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);

  switch (parser->tokens[parser->pos].type) {
  case SOLC_TOKENTYPE_SEMI: {
    return solc_ast_none_create(parser->pos++);
  }

  case SOLC_TOKENTYPE_LCBRACK: {
    return solc_parser_parse_stmt_block(parser);
  }

  case SOLC_TOKENTYPE_ID: {
    parser_stmt_func_t func =
      parser_context_get_stmt_func(parser->tokens[parser->pos].value);

    if (func != nullptr) {
      return func(parser);
    } else if (parser_context_is_qualifier(parser->tokens[parser->pos].value)) {
      return solc_parser_parse_decldef(parser, nullptr);
    }

    if (solc_parser_peek(parser, parser->pos + 1) == SOLC_TOKENTYPE_COLON) {
      if (!parser->tokens[parser->pos].has_whitespace_after &&
          solc_parser_peek(parser, parser->pos + 2) == SOLC_TOKENTYPE_COLON &&
          solc_parser_peek(parser, parser->pos + 3) != SOLC_TOKENTYPE_LPAREN) {
        break;
      }
      return solc_parser_parse_decldef(parser, nullptr);
    }
  } break;

  case SOLC_TOKENTYPE_LBRACK: {
    solc_ast_t *attrib_list = solc_parser_parse_attribute_list(parser);
    return solc_parser_parse_decldef(parser, attrib_list);
  }

  case SOLC_TOKENTYPE_AT: {
    return solc_parser_parse_stmt_label(parser);
  }

  default:
    break;
  }

  solc_ast_t *out =
    solc_parser_parse_stmt_expr_or_generic_func_or_namespace(parser);
  if (parser->errored) {
    if (out != nullptr)
      solc_ast_destroy(out);
    parser->pos++;
    return nullptr;
  }

  return out;
}

solc_ast_t *
solc_parser_parse_stmt_expr_or_generic_func_or_namespace(solc_parser_t *parser)
{
  if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_ID &&
      solc_parser_peek(parser, parser->pos + 1) == SOLC_TOKENTYPE_LARROW) {
    sz old_pos = parser->pos;
    b8 old_errored = parser->errored;
    sz old_errors_v_len = vector_get_length(parser->errors_v);
    solc_parser_error_t *old_errors_v = vector_copy(parser->errors_v);

    // Try parse generic function definition
    solc_ast_t *out = solc_parser_parse_def_func_generic(
      parser, nullptr, SOLC_AST_FUNC_TYPE_DEFAULT);

    if (old_errored == parser->errored ||
        old_errors_v_len == vector_get_length(parser->errors_v)) {
      // Success
      vector_destroy(old_errors_v);
      return out;
    }

    // Fail
    // Reset parser to its previous state ...
    vector_destroy(parser->errors_v);
    parser->pos = old_pos;
    parser->errored = old_errored;
    parser->errors_v = vector_copy(old_errors_v);

    // ... and parse expression statement.
  }

  return solc_parser_parse_stmt_expr(parser);
}
