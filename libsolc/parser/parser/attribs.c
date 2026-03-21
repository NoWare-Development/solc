#include "parser/ast/ast_group_none.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_attribute(solc_parser_t *parser)
{
  solc_ast_t *out_attrib =
    solc_ast_attribute_create(parser->pos, parser->tokens[parser->pos].value);
  parser->pos++;

  if (parser->pos < parser->tokens_num &&
      parser->tokens[parser->pos].type == SOLC_TOKENTYPE_LPAREN) {
    parser->pos++;
    while (parser->pos < parser->tokens_num) {
      if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RPAREN)
        break;

      solc_ast_t *expr = solc_parser_parse_expr(parser, false);
      solc_ast_attribute_add_argument(out_attrib, expr);

      VERIFY_POS(parser, parser->pos);
      if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RPAREN)
        continue;

      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_COMMA);
      parser->pos++;
      VERIFY_POS(parser, parser->pos);
    }

    VERIFY_POS(parser, parser->pos);
    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_RPAREN);
    parser->pos++;
  }

  return out_attrib;
}

solc_ast_t *solc_parser_parse_attribute_list(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LBRACK);
  VERIFY_WHITESPACE(parser, parser->pos,
                    parser->tokens[parser->pos].has_whitespace_after, false,
                    SOLC_TOKENTYPE_LBRACK);

  solc_ast_t *out_attrib_list = solc_ast_attribute_list_create(parser->pos++);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LBRACK);

  parser->pos++;
  while (parser->pos < parser->tokens_num) {
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RBRACK)
      break;

    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_ID);

    solc_ast_t *attrib = solc_parser_parse_attribute(parser);
    solc_ast_attribute_list_add_attribute(out_attrib_list, attrib);

    VERIFY_POS(parser, parser->pos);
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RBRACK)
      continue;

    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_COMMA);
    parser->pos++;
    VERIFY_POS(parser, parser->pos);
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RBRACK);
  VERIFY_WHITESPACE(parser, parser->pos,
                    parser->tokens[parser->pos].has_whitespace_after, false,
                    SOLC_TOKENTYPE_RBRACK);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RBRACK);

  parser->pos++;

  return out_attrib_list;
}

solc_ast_t *solc_parser_parse_attribute_list_optional(solc_parser_t *parser)
{
  return parser->pos + 1 < parser->tokens_num &&
             parser->tokens[parser->pos].type == SOLC_TOKENTYPE_LBRACK &&
             !parser->tokens[parser->pos].has_whitespace_after &&
             parser->tokens[parser->pos + 1].type == SOLC_TOKENTYPE_LBRACK ?
           solc_parser_parse_attribute_list(parser) :
           nullptr;
}
