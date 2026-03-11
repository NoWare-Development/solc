#include "parser/ast/ast_group_stmt.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"
#include <string.h>

solc_ast_t *solc_parser_parse_stmt_switch(solc_parser_t *parser)
{
  sz switch_pos = parser->pos++;

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LPAREN);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);

  solc_ast_t *in_expr = solc_parser_parse_expr(parser, false);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RPAREN);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LCBRACK);

  solc_ast_t *stmt_switch = solc_ast_stmt_switch_create(switch_pos, in_expr);

  parser->pos++;
  while (parser->pos < parser->tokens_num) {
    if (parser->tokens[parser->pos].type != SOLC_TOKENTYPE_ID) {
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_RCBRACK);
      break;
    }

    const char *value = parser->tokens[parser->pos].value;

    if (strcmp(value, "default") == 0) {
      sz default_pos = parser->pos++;
      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_COLON);

      parser->pos++;
      VERIFY_POS(parser, parser->pos);

      solc_ast_t *stmt_block = solc_parser_parse_stmt_block(parser);

      solc_ast_t *stmt_default =
        solc_ast_stmt_default_create(default_pos, stmt_block);

      solc_ast_stmt_switch_add_case(stmt_switch, stmt_default);

      continue;
    }

    VERIFY_VALUE(parser, parser->pos, value, "case");

    sz case_pos = parser->pos++;
    VERIFY_POS(parser, parser->pos);

    solc_ast_t *case_expr = solc_parser_parse_expr(parser, false);
    VERIFY_POS(parser, parser->pos);
    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_COLON);

    parser->pos++;
    VERIFY_POS(parser, parser->pos);

    solc_ast_t *stmt_block = solc_parser_parse_stmt_block(parser);

    solc_ast_t *stmt_case =
      solc_ast_stmt_case_create(case_pos, case_expr, stmt_block);

    solc_ast_stmt_switch_add_case(stmt_switch, stmt_case);
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RCBRACK);

  parser->pos++;

  return stmt_switch;
}
