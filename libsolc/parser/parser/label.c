#include "parser/ast/ast_group_stmt.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

// TODO: Remove this `stmt` from function name.
solc_ast_t *solc_parser_parse_stmt_label(solc_parser_t *parser)
{
  sz pos = parser->pos;

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_AT);

  parser->pos++;

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);

  // TODO: Remove `stmt` from function name.
  solc_ast_t *out_label =
    solc_ast_stmt_label_create(pos, parser->tokens[parser->pos++].value);

  return out_label;
}
