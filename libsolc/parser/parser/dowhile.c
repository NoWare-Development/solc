#include "parser/ast/ast_group_stmt.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_stmt_dowhile(solc_parser_t *parser)
{
  sz dowhile_pos = parser->pos++;
  VERIFY_POS(parser, parser->pos);

  solc_ast_t *stmt = solc_parser_parse_stmt(parser);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);
  VERIFY_VALUE(parser, parser->pos, parser->tokens[parser->pos].value, "while");

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LPAREN);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  solc_ast_t *cond_expr = solc_parser_parse_expr(parser, false);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RPAREN);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_SEMI);

  parser->pos++;

  return solc_ast_stmt_dowhile_create(dowhile_pos, cond_expr, stmt);
}
