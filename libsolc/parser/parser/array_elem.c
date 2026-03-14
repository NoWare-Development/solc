#include "parser/ast/ast_group_expr_operand.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/parser.h"
#include "solc/parser/ast.h"

solc_ast_t *solc_parser_parse_expr_operand_array_element(solc_parser_t *parser,
                                                         solc_ast_t *array_ast)
{
  solc_tokentype_t curtype = solc_parser_peek(parser, parser->pos);
  solc_ast_t *current = array_ast;
  solc_ast_t *buf = nullptr;
  while (curtype == SOLC_TOKENTYPE_LBRACK && parser->pos < parser->tokens_num) {
    sz array_elem_start = parser->pos++;

    VERIFY_POS(parser, parser->pos);

    solc_ast_t *expr_ast = solc_parser_parse_expr(parser, false);
    buf = solc_ast_expr_operand_array_element_create(array_elem_start, expr_ast,
                                                     current);
    VERIFY_POS(parser, parser->pos);
    VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
                 SOLC_TOKENTYPE_RBRACK);
    parser->pos++;

    current = buf;
    curtype = solc_parser_peek(parser, parser->pos);
  }

  return current;
}
