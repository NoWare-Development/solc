#include "parser/ast/ast_group_stmt.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_stmt_label(solc_parser_t *parser)
{
  sz pos = parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);

  const char *label_name = parser->tokens[parser->pos++].value;

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_COLON);

  parser->pos++;

  return solc_ast_stmt_label_create(pos, label_name);
}
