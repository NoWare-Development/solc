#include "parser/ast/ast_group_none.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_vismarker(solc_parser_t *parser)
{
  solc_ast_t *vismarker =
    solc_ast_vismarker_create(parser->pos, parser->tokens[parser->pos].value);

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_COLON);

  parser->pos++;
  return vismarker;
}
