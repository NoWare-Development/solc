#ifndef __SOLC_ERROR_HANDLER_H__
#define __SOLC_ERROR_HANDLER_H__

#include <solc/parser/parser.h>
#include <solc/lexer/token.h>

typedef struct {
  const char *filename;

  const char *src;
  sz src_len;

  solc_token_t *tokens;
  sz n_tokens;

  sz nol;
  sz lnoffset;
} error_handler_t;

error_handler_t error_handler_create(const char *filename, const char *src,
                                     solc_token_t *tokens, sz n);

void error_handler_report_failed_to_open(const char *filepath, s32 errno_n);
b8 error_handler_handle_invalid_tokens(error_handler_t *handler);
b8 error_handler_handle_parser_errors(error_handler_t *handler,
                                      solc_parser_error_t *errors, sz n);

#endif // __SOLC_ERROR_HANDLER_H__
