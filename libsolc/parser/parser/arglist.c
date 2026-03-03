#include "parser/ast/ast_group_none.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/parser.h"
#include "solc/parser/ast.h"

solc_ast_t *solc_parser_parse_func_arglist(solc_parser_t *parser)
{
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LPAREN);

  solc_ast_t *arglist = solc_ast_func_arglist_create(parser->pos++);
  VERIFY_POS(parser, parser->pos);
  if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RPAREN)
    goto func_arglist_parse_end;

  while (parser->pos < parser->tokens_num) {
    solc_ast_t *arg = solc_parser_parse_func_arg(parser);
    solc_ast_func_arglist_add_element(arglist, arg);

    VERIFY_POS(parser, parser->pos);
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RPAREN)
      break;

    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_COMMA);

    parser->pos++;
    VERIFY_POS(parser, parser->pos);
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RPAREN) {
      solc_parser_add_error(parser, SOLC_PARSER_ERROR_TYPE_UNEXPECTED,
                            parser->pos, 1, SOLC_TOKENTYPE_ERR);
      return nullptr;
    }
  }

func_arglist_parse_end:
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RPAREN);

  parser->pos++;
  return arglist;
}

solc_ast_t *solc_parser_parse_func_arg(solc_parser_t *parser)
{
  switch (parser->tokens[parser->pos].type) {
  case SOLC_TOKENTYPE_PERIOD: { // Variadic
    VERIFY_POS(parser, parser->pos + 1);
    VERIFY_POS(parser, parser->pos + 2);
    VERIFY_TOKEN(parser, parser->pos + 1, parser->tokens[parser->pos + 1].type,
                 SOLC_TOKENTYPE_PERIOD);
    VERIFY_TOKEN(parser, parser->pos + 2, parser->tokens[parser->pos + 2].type,
                 SOLC_TOKENTYPE_PERIOD);

    solc_ast_t *variadic = solc_ast_variadic_create(parser->pos);
    parser->pos += 3;
    return variadic;
  }

  case SOLC_TOKENTYPE_ID: {
    if (solc_parser_is_qualifier(parser->tokens[parser->pos].value)) {
      sz qualifier_pos = parser->pos++;
      const char *qualifier_name = parser->tokens[parser->pos].value;
      solc_ast_t *arg = solc_parser_parse_func_arg(parser);
      return solc_ast_qualifier_create(qualifier_pos, qualifier_name, arg);
    }
  } break;

  default:
    break;
  }

  return solc_parser_parse_decldef(parser);
}
