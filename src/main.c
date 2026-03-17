#include <errno.h>
#define ARGUMENTS                                                 \
  BOOLEAN_ARG(show_help, "--help", "-h", "Display this message")  \
  BOOLEAN_ARG(show_version, "--version", "-v", "Display version") \
  PREFIX_ARG(link_against, "-l", "Link against", "lib")           \
  VALUE_ARG(output, "--output", "-o", "Output", "file")

#include "args.h"
#include "errorhandler.h"
#include <solc/init.h>
#include <solc/parser/parser.h>
#include <solc/lexer/lexer.h>
#include <solc/lexer/token.h>
#include <solc/defs.h>
#include <stdio.h>
#include <stdlib.h>
#include <solc/parser/ast.h>

#define SOLC_VERSION 0.0.1
#define SOLC_COPYRIGHT_YEAR 2026
#define SOLC_COMPILER_DEVELOPER "NoWare-Development"

static const char *_version_message = "solc (SOLC) " __SOLC_MACRO_STR(
  SOLC_VERSION) "\n"
                "Copyright (C) " __SOLC_MACRO_STR(
                  SOLC_COPYRIGHT_YEAR) " " SOLC_COMPILER_DEVELOPER "\n"
                                       "This is free software; see the source "
                                       "for copying conditions (not ready yet)."
                                       "  There is NO\n"
                                       "warranty; not even for MERCHANTABILITY"
                                       " or FITNESS FOR A PARTICULAR PURPOSE.";

s32 main(s32 argc, char **argv)
{
  solc_init();

  args_t args = { 0 };
  if (!args_parse(&args, argc, argv))
    return -1;

  if (args.show_help) {
    args_help("Usage: solc [options] file...");
    return 0;
  } else if (args.show_version) {
    puts(_version_message);
    return 0;
  }

  if (args.num_dangling == 0) {
    fprintf(stderr, "No sources were provided.\n");
    return -1;
  }

  for (s32 i = 0; i < args.num_dangling; i++) {
    const char *filepath = argv[args.danlings[i]];

    FILE *f = fopen(filepath, "r");
    if (f == nullptr) {
      error_handler_report_failed_to_open(filepath, errno);
      return -2;
    }

    fseek(f, 0, SEEK_END);
    s32 size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *src = calloc(size + 1, sizeof(char));
    fread(src, sizeof(char), size, f);
    fclose(f);

    solc_lexer_t *lexer = solc_lexer_create(src);
    sz tokens_num;
    solc_token_t *tokens = solc_lexer_tokenize(lexer, &tokens_num);
    for (sz i = 0; i < tokens_num; i++) {
      char buf[0x80] = { 0 };
      solc_token_to_string(buf, sizeof(buf) - 1, &tokens[i]);
      printf("(%zu) %s\n", i, buf);
    }

    error_handler_t handler =
      error_handler_create(argv[args.danlings[i]], src, tokens, tokens_num);
    if (!error_handler_handle_invalid_tokens(&handler)) {
      return -3;
    }

    solc_parser_t parser = solc_parser_create(tokens, tokens_num);
    solc_ast_t *root = solc_parser_parse(&parser);

#ifdef _DEBUG
    sz n = 1 << 16;
    char *buf = malloc(sizeof(char) * (n + 1));
    solc_ast_to_string(buf, n, root);
    printf("%s", buf);
    free(buf);
#endif

    sz parser_errors_num;
    solc_parser_error_t *parser_errors =
      solc_parser_get_errors(&parser, &parser_errors_num);

    if (!error_handler_handle_parser_errors(&handler, parser_errors,
                                            parser_errors_num)) {
      return -4;
    }

    solc_ast_destroy(root);
    solc_parser_destroy(&parser);

    solc_lexer_destroy(lexer);

    free(src);
  }

  solc_deinit();

  return 0;
}
