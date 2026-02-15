#include "parser/ast/ast_group_none.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"
#include "parser/parser_private.h"
#include <string.h>

solc_ast_t *solc_parser_parse_top(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  solc_token_t cur = parser->tokens[parser->pos];
  switch (cur.type) {
  case SOLC_TOKENTYPE_SEMI:
    return solc_ast_none_create(parser->pos++);

  case SOLC_TOKENTYPE_ID:
    if (strcmp(cur.value, "import") == 0) {
      return solc_parser_parse_import(parser);
    }
    // TODO: more toplevel statements.

    // return solc_parser_parse_decldef(parser);

  default:
    break;
  }

  solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_UNEXPECTED, parser->pos,
                        1);
  return nullptr;
}
