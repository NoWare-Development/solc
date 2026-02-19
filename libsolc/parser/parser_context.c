#include "parser/parser_context.h"
#include "containers/trie.h"
#include "parser/parser_private.h"
#include "solc/defs.h"
#include <stdlib.h>

typedef struct {
  trie_t *toplevel_funcptrs;
  trie_t *stmt_funcptrs;
  trie_t *struct_funcptrs;
  trie_t *union_funcptrs;
  trie_t *qualifiers;
  sz refcount;
} parser_context_t;

static parser_context_t *ctx = nullptr;

void parser_context_create(void)
{
  if SOLC_LIKELY (ctx != nullptr) {
    ctx->refcount++;
    return;
  }

  ctx = malloc(sizeof(parser_context_t));
  ctx->toplevel_funcptrs = trie_create();
  ctx->stmt_funcptrs = trie_create();
  ctx->struct_funcptrs = trie_create();
  ctx->union_funcptrs = trie_create();

  // TODO: uncomment these lines when all these functions are implemented.

  // trie_insert(ctx->toplevel_funcptrs, "enum", solc_parser_parse_enum);
  // trie_insert(ctx->toplevel_funcptrs, "typedef", solc_parser_parse_typedef);
  trie_insert(ctx->toplevel_funcptrs, "struct", solc_parser_parse_struct);
  // trie_insert(ctx->toplevel_funcptrs, "union", solc_parser_parse_union);
  trie_insert(ctx->toplevel_funcptrs, "import", solc_parser_parse_import);
  // trie_insert(ctx->toplevel_funcptrs, "extern", solc_parser_parse_extern);
  // trie_insert(ctx->toplevel_funcptrs, "export", solc_parser_parse_export);

  // trie_insert(ctx->stmt_funcptrs, "struct", solc_parser_parse_struct);
  // trie_insert(ctx->stmt_funcptrs, "union", solc_parser_parse_union);
  // trie_insert(ctx->stmt_funcptrs, "enum", solc_parser_parse_enum);
  // trie_insert(ctx->stmt_funcptrs, "return", solc_parser_parse_stmt_return);
  // trie_insert(ctx->stmt_funcptrs, "goto", solc_parser_parse_stmt_goto);
  // trie_insert(ctx->stmt_funcptrs, "break", solc_parser_parse_stmt_break);
  // trie_insert(ctx->stmt_funcptrs, "continue", solc_parser_parse_stmt_continue);
  // trie_insert(ctx->stmt_funcptrs, "while", solc_parser_parse_stmt_while);
  // trie_insert(ctx->stmt_funcptrs, "for", solc_parser_parse_stmt_for);
  // trie_insert(ctx->stmt_funcptrs, "do", solc_parser_parse_stmt_dowhile);
  // trie_insert(ctx->stmt_funcptrs, "switch", solc_parser_parse_stmt_switch);
  // trie_insert(ctx->stmt_funcptrs, "defer", solc_parser_parse_stmt_defer);
  // trie_insert(ctx->stmt_funcptrs, "if", solc_parser_parse_stmt_if);
  // trie_insert(ctx->stmt_funcptrs, "typedef", solc_parser_parse_typedef);

  // trie_insert(ctx->struct_funcptrs, "typedef", solc_parser_parse_typedef);
  // trie_insert(ctx->struct_funcptrs, "enum", solc_parser_parse_enum);
  trie_insert(ctx->struct_funcptrs, "struct", solc_parser_parse_struct);
  // trie_insert(ctx->struct_funcptrs, "union", solc_parser_parse_union);

  // trie_insert(ctx->union_funcptrs, "typedef", solc_parser_parse_typedef);
  // trie_insert(ctx->union_funcptrs, "struct", solc_parser_parse_struct);
  // trie_insert(ctx->union_funcptrs, "union", solc_parser_parse_union);

  trie_insert(ctx->qualifiers, "inline", (void *)1);
  trie_insert(ctx->qualifiers, "persist", (void *)1);
  trie_insert(ctx->qualifiers, "local", (void *)1);

  ctx->refcount = 1;
}

void parser_context_destroy(void)
{
  SOLC_ASSUME(ctx != nullptr && ctx->refcount > 0);

  ctx->refcount--;
  if (ctx->refcount == 0) {
    trie_destroy(ctx->toplevel_funcptrs);
    trie_destroy(ctx->stmt_funcptrs);
    trie_destroy(ctx->struct_funcptrs);
    trie_destroy(ctx->union_funcptrs);
    trie_destroy(ctx->qualifiers);
    free(ctx);
    ctx = nullptr;
  }
}

parser_toplevel_func_t parser_context_get_toplevel_func(const char *str)
{
  SOLC_ASSUME(ctx != nullptr && str != nullptr);
  return trie_get(ctx->toplevel_funcptrs, str);
}

parser_stmt_func_t parser_context_get_stmt_func(const char *str)
{
  SOLC_ASSUME(ctx != nullptr && str != nullptr);
  return trie_get(ctx->stmt_funcptrs, str);
}

parser_struct_func_t parser_context_get_struct_func(const char *str)
{
  SOLC_ASSUME(ctx != nullptr && str != nullptr);
  return trie_get(ctx->struct_funcptrs, str);
}

parser_union_func_t parser_context_get_union_func(const char *str)
{
  SOLC_ASSUME(ctx != nullptr && str != nullptr);
  return trie_get(ctx->union_funcptrs, str);
}

b8 parser_context_is_qualifier(const char *str)
{
  SOLC_ASSUME(ctx != nullptr && str != nullptr);
  return trie_get(ctx->qualifiers, str) != 0;
}
