#include "parser/ast/ast_group_none.h"
#include "parser/parser_context.h"
#include "parser/parser_private.h"
#include "solc/lexer/token.h"
#include "solc/parser/ast.h"
#include "solc/parser/parser.h"

solc_ast_t *solc_parser_parse_union(solc_parser_t *parser)
{
  parser->pos++;
  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_ID);

  const char *union_name = parser->tokens[parser->pos].value;
  const sz union_pos = parser->pos;

  parser->pos++;
  solc_ast_t *attrib_list = solc_parser_parse_attribute_list_optional(parser);

  solc_ast_t *union_ast =
    solc_ast_union_create(union_pos, union_name, attrib_list);

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_LCBRACK);

  parser->pos++;
  while (parser->pos < parser->tokens_num) {
    // NOTE: Cannot check if 'child_ast' is a null-pointer or not,
    // because if parse function failed, it can output a null-pointer.
    b8 parsed_child = false;
    solc_ast_t *child_ast = nullptr;
    if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_RCBRACK)
      break;
    else if (parser->tokens[parser->pos].type == SOLC_TOKENTYPE_ID) {
      parser_union_func_t union_func =
        parser_context_get_union_func(parser->tokens[parser->pos].value);
      if (union_func != nullptr) {
        child_ast = union_func(parser);
        parsed_child = true;
      }
    }

    if (!parsed_child)
      child_ast = solc_parser_parse_decldef(parser);

    solc_ast_union_add_child(union_ast, child_ast);
  }

  VERIFY_POS(parser, parser->pos);
  VERIFY_TOKEN(parser, parser->pos, parser->tokens[parser->pos].type,
               SOLC_TOKENTYPE_RCBRACK);
  parser->pos++;

  return union_ast;
}
