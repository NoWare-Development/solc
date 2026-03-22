#include "parser/ast/ast_group_none.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_typedef(solc_parser_t *parser)
{
  sz typedef_pos = parser->pos++;

  solc_ast_t *attrib_list = solc_parser_parse_attribute_list_optional(parser);

  VERIFY_POS(parser, parser->pos);

  solc_ast_t *aliased_type = solc_parser_parse_type(parser);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);
  const char *alias = parser->tokens[parser->pos].value;

  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_SEMI);

  parser->pos++;

  return solc_ast_typedef_create(typedef_pos, aliased_type, alias, attrib_list);
}
