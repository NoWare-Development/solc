#include "parser/ast/ast_group_none.h"
#include "parser/parser_private.h"
#include "solc/defs.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_enum(solc_parser_t *parser)
{
  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);

  const char *enum_name = parser->tokens[parser->pos].value;
  const sz enum_pos = parser->pos;

  parser->pos++;
  solc_ast_t *attrib_list = solc_parser_parse_attribute_list_optional(parser);
  solc_ast_t *enum_ast = solc_ast_enum_create(enum_pos, enum_name, attrib_list);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LCBRACK);

  parser->pos++;
  while (parser->pos < parser->tokens_num) {
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RCBRACK)
      break;

    solc_ast_t *element = solc_parser_parse_enum_element(parser);
    solc_ast_enum_add_element(enum_ast, element);

    VERIFY_POS(parser, parser->pos);
    if (parser->tokens[parser->pos].type != SOLC_TOKENTYPE_COMMA) {
      VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                   SOLC_TOKENTYPE_RCBRACK);
    } else {
      parser->pos++;
    }
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RCBRACK);
  parser->pos++;

  return enum_ast;
}

solc_ast_t *solc_parser_parse_enum_element(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);

  const char *element_name = parser->tokens[parser->pos].value;
  sz element_pos = parser->pos++;

  solc_ast_t *expr = nullptr;

  solc_tokentype_t next = solc_parser_peek(parser, parser->pos);
  if (next == SOLC_TOKENTYPE_EQ) {
    parser->pos++;
    VERIFY_POS(parser, parser->pos);
    expr = solc_parser_parse_expr(parser, false);
  }

  return solc_ast_enum_element_create(element_pos, element_name, expr);
}
