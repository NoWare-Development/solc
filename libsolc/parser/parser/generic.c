#include "parser/ast/ast_group_generic.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *
solc_parser_parse_generic_placeholder_type_list(solc_parser_t *parser)
{
  solc_ast_t *generic_placeholder_type_list_ast =
    solc_ast_generic_placeholder_type_list_create(parser->pos++);

  VERIFY_POS(parser, parser->pos);
  while (parser->pos < parser->tokens_num) {
    solc_token_t cur = parser->tokens[parser->pos];
    if (cur.type == SOLC_TOKENTYPE_RARROW)
      break;

    VERIFY_TOKEN(parser, parser->pos, cur.type, SOLC_TOKENTYPE_ID);

    solc_ast_t *generic_placeholder_type_ast =
      solc_ast_generic_placeholder_type_create(parser->pos++, cur.value);
    solc_ast_generic_placeholder_type_list_add_placeholder_type(
      generic_placeholder_type_list_ast, generic_placeholder_type_ast);
    solc_tokentype_t next = solc_parser_peek(parser, parser->pos);
    if (next != SOLC_TOKENTYPE_RARROW) {
      VERIFY_POS(parser, parser->pos);
      VERIFY_TOKEN(parser, parser->pos, next, SOLC_TOKENTYPE_COMMA);
      parser->pos++;
    }
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RARROW);
  parser->pos++;

  return generic_placeholder_type_list_ast;
}
