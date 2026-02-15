#include "parser/ast/ast_group_none.h"
#include "parser/parser_context.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"
#include "parser/parser_private.h"

solc_ast_t *solc_parser_parse_top(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  solc_token_t cur = parser->tokens[parser->pos];
  switch (cur.type) {
  case SOLC_TOKENTYPE_SEMI:
    return solc_ast_none_create(parser->pos++);

  case SOLC_TOKENTYPE_ID: {
    parser_toplevel_func_t toplevel_func =
      parser_context_get_toplevel_func(cur.value);
    if (toplevel_func != nullptr) {
      return toplevel_func(parser);
    } else if (solc_parser_peek(parser, parser->pos + 1) ==
               SOLC_TOKENTYPE_LARROW) {
      // TODO:
      // return solc_parser_parse_generic_function(parser);
    }

    // TODO:
    // return solc_parser_parse_decldef(parser);
  }

  default:
    break;
  }

  solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_UNEXPECTED, parser->pos,
                        1);
  return nullptr;
}
