#include "parser/parser_private.h"
#include "parser/ast_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_stmt_for(solc_parser_t *parser)
{
  sz for_pos = parser->pos++;

  solc_ast_t *attrib_list = solc_parser_parse_attribute_list_optional(parser);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LPAREN);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);

  solc_ast_t *init_stmt;
  if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_SEMI) {
    init_stmt = nullptr;
    parser->pos++;
  } else {
    init_stmt = solc_parser_parse_stmt(parser);
  }

  VERIFY_POS(parser, parser->pos);

  solc_ast_t *cond_expr = parser->tokens[parser->pos].type !=
                              SOLC_TOKENTYPE_SEMI ?
                            solc_parser_parse_expr(parser, false) :
                            nullptr;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_SEMI);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);

  solc_ast_t *expr = parser->tokens[parser->pos].type != SOLC_TOKENTYPE_RPAREN ?
                       solc_parser_parse_expr(parser, true) :
                       nullptr;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RPAREN);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);

  solc_ast_t *stmt = solc_parser_parse_stmt(parser);

  return solc_ast_stmt_for_create(for_pos, init_stmt, cond_expr, expr, stmt,
                                  attrib_list);
}
