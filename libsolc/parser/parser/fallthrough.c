#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"
#include "parser/ast_private.h"

solc_ast_t *solc_parser_parse_stmt_fallthrough(solc_parser_t *parser)
{
  sz pos = parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_SEMI);
  parser->pos++;
  return solc_ast_stmt_fallthrough_create(pos);
}
