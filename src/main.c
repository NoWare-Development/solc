#include "args.h"
#include "info.h"
#include <solc/init.h>
#include <solc/parser/parser.h>
#include <solc/lexer/lexer.h>
#include <solc/lexer/token.h>
#include <solc/defs.h>
#include <stdio.h>
#include <stdlib.h>
#include <solc/parser/ast.h>

s32 main(s32 argc, char **argv)
{
  if (argc < 2) {
    display_help();
    return 0;
  }

  solc_init();

  args_set(argc, argv);

  if (arg_present("--help", "-h")) {
    display_help();
    return 0;
  } else if (arg_present("--version", "-v")) {
    display_version();
    return 0;
  }

  s32 dangling_arguments_num = arg_get_dangling(nullptr);
  if (dangling_arguments_num == 0) {
    display_help();
    return 0;
  }
  s32 *dangling_arguments = malloc(sizeof(s32) * dangling_arguments_num);
  arg_get_dangling(dangling_arguments);

  for (s32 i = 0; i < dangling_arguments_num; i++) {
    FILE *f = fopen(argv[dangling_arguments[i]], "r");
    fseek(f, 0, SEEK_END);
    s32 size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *src = calloc(sizeof(char), size + 1);
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

    solc_parser_t parser = solc_parser_create(tokens, tokens_num);
    solc_ast_t *root = solc_parser_parse(&parser);

    sz n = 8192;
    char *buf = malloc(sizeof(char) * (n + 1));
    solc_ast_to_string(buf, n, root);
    printf("%s", buf);
    free(buf);

    solc_ast_destroy(root);
    solc_parser_destroy(&parser);

    solc_lexer_destroy(lexer);

    free(src);
  }

  solc_deinit();

  return 0;
}
