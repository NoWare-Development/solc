#include "parser/ast/ast_group_stmt.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"
#include <string.h>

solc_ast_t *solc_parser_parse_stmt_if(solc_parser_t *parser)
{
  sz pos = parser->pos++;

  solc_ast_t *attrib_list = solc_parser_parse_attribute_list_optional(parser);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LPAREN);

  parser->pos++;

  solc_ast_t *expr = solc_parser_parse_expr(parser, false);
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RPAREN);

  parser->pos++;

  solc_ast_t *stmt = solc_parser_parse_stmt(parser);
  solc_ast_t *else_stmt =
    parser->pos < parser->tokens_num &&
        parser->tokens[parser->pos].type == SOLC_TOKENTYPE_ID &&
        strcmp(parser->tokens[parser->pos].value, "else") == 0 ?
      solc_parser_parse_stmt_else(parser) :
      nullptr;

  return solc_ast_stmt_if_create(pos, expr, attrib_list, stmt, else_stmt);
}
