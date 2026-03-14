#include "parser/ast/ast_group_initlist.h"
#include "parser/ast/ast_group_none.h"
#include "parser/parser_private.h"
#include "solc/defs.h"
#include "solc/lexer/token.h"
#include "solc/parser/parser.h"
#include "solc/parser/ast.h"

solc_ast_t *solc_parser_parse_initlist(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LCBRACK);

  solc_ast_t *initlist = solc_ast_initlist_create(parser->pos++);

  while (parser->pos < parser->tokens_num) {
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RCBRACK)
      break;

    solc_ast_initlist_add_element(initlist,
                                  solc_parser_parse_initlist_entry(parser));

    VERIFY_POS(parser, parser->pos);
    if (parser->tokens[parser->pos].type != SOLC_TOKENTYPE_RCBRACK) {
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_COMMA);
      parser->pos++;
    }
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RCBRACK);

  parser->pos++;

  return initlist;
}

solc_ast_t *solc_parser_parse_initlist_entry(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);

  sz pos = parser->pos;
  int ast_type = 0;
  solc_ast_t *array_idx_expr_ast = nullptr;
  const char *explicit_entry_name = nullptr;

  if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_PERIOD) {
    ast_type = 1;

    parser->pos++;
    VERIFY_POS(parser, parser->pos);
    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_ID);
    explicit_entry_name = parser->tokens[parser->pos].value;

    parser->pos++;
    VERIFY_POS(parser, parser->pos);

    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_LBRACK) {
      ast_type = 2;

      parser->pos++;
      array_idx_expr_ast = solc_parser_parse_expr(parser, false);

      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_RBRACK);

      parser->pos++;
      VERIFY_POS(parser, parser->pos);
    }

    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_EQ);

    parser->pos++;
    VERIFY_POS(parser, parser->pos);
  }

  solc_ast_t *expr = parser->tokens[parser->pos].type ==
                         SOLC_TOKENTYPE_LCBRACK ?
                       solc_parser_parse_initlist(parser) :
                       solc_parser_parse_expr(parser, false);

  solc_ast_t *out;
  switch (ast_type) {
  case 0:
    out = solc_ast_initlist_entry_create(pos, expr);
    break;
  case 1:
    out =
      solc_ast_initlist_entry_explicit_create(pos, explicit_entry_name, expr);
    break;
  case 2:
    out = solc_ast_initlist_entry_explicit_array_element_create(
      pos, explicit_entry_name, array_idx_expr_ast, expr);
    break;
  default:
    SOLC_NOREACH();
  }

  return out;
}
