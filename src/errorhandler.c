#include "errorhandler.h"
#include "color.h"
#include <solc/defs.h>
#include <solc/lexer/token.h>
#include <solc/parser/parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static sz get_ulen(u32 n);
static inline void get_message_start(const char *filename, sz line, sz start,
                                     const char *msg, esccolor_t esccolor,
                                     escgraphics_t escgraphics, char *out,
                                     sz n);
static void get_highlighted_token(error_handler_t *handler,
                                  const solc_token_t *tok, esccolor_t esccolor,
                                  escgraphics_t escgraphics, char *out, sz n,
                                  const char *msg_after);
static void get_line(const char *src, sz src_len, sz line, char *out, sz n);
static void get_parser_error_reason(const solc_parser_error_t *error,
                                    const solc_token_t *tokens, char *out,
                                    sz n);

static const char *token_display(solc_tokentype_t type);
static const char *token_to_value(const solc_token_t *token);

static void insert_at(char *dst, const char *src, sz i);

error_handler_t error_handler_create(const char *filename, const char *src,
                                     solc_token_t *tokens, sz n)
{
  error_handler_t handler = {
    .filename = filename,
    .src = src,
    .src_len = strlen(src),
    .tokens = tokens,
    .n_tokens = n,
    .nol = 0,
  };

  for (; *src; src++)
    if (*src == '\n')
      handler.nol++;

  handler.lnoffset = get_ulen(handler.nol);

  return handler;
}

void error_handler_report_failed_to_open(const char *filepath, s32 errno_n)
{
  // TODO: use 'errno_n'
  fprintf(stderr,
          ESCGRAPHICS_BOLD
          "solc: " ESCCOLOR_RED "fatal error: " ESC_RESET
          "%s: No such file or directory\ncompilation terminated.\n",
          filepath);
}

b8 error_handler_handle_invalid_tokens(error_handler_t *handler)
{
  b8 result = true;

  for (sz i = 0; i < handler->n_tokens; i++) {
    const solc_token_t *token = &handler->tokens[i];

    if (token->type != SOLC_TOKENTYPE_ERR)
      continue;

    char msg_start[1024] = { 0 };
    get_message_start(handler->filename, token->line + 1,
                      token->end - token->len + 1, "error", ESCCOLOR_RED,
                      ESCGRAPHICS_BOLD, msg_start, 1023);

    char highlighted_token[1024] = { 0 };
    get_highlighted_token(handler, token, ESCCOLOR_RED, ESCGRAPHICS_BOLD,
                          highlighted_token, 1023, nullptr);

    fprintf(stderr,
            "%sunidentified token '" ESCGRAPHICS_BOLD "%s" ESC_RESET "'\n%s",
            msg_start, token->value, highlighted_token);

    result = false;
  }

  return result;
}

b8 error_handler_handle_parser_errors(error_handler_t *handler,
                                      solc_parser_error_t *errors, sz n)
{
  if (!n)
    return true;

  for (; n; errors++, n--) {
    solc_parser_error_t *error = errors;

    if (error->pos + error->len - 1 >= handler->n_tokens &&
        error->type != SOLC_PARSER_ERROR_TYPE_EXPECTED)
      continue;

    char msg_start[1024] = { 0 };
    if (error->type == SOLC_PARSER_ERROR_TYPE_EXPECTED) {
      char last_line[1024] = { 0 };
      get_line(handler->src, handler->src_len, handler->nol, last_line, 1023);
      get_message_start(handler->filename, handler->nol, strlen(last_line),
                        "error", ESCCOLOR_RED, ESCGRAPHICS_BOLD, msg_start,
                        1023);
    } else {
      get_message_start(
        handler->filename, handler->tokens[error->pos].line,
        handler->tokens[error->pos].end - handler->tokens[error->pos].len + 1,
        "error", ESCCOLOR_RED, ESCGRAPHICS_BOLD, msg_start, 1023);
    }

    char error_reason[1024] = { 0 };
    get_parser_error_reason(error, handler->tokens, error_reason, 1023);

    char hint[48] = { 0 };
    if (error->expected != SOLC_TOKENTYPE_ERR) {
      snprintf(hint, 47, "expected %s", token_display(error->expected));
    }

    sz token_pos = error->type != SOLC_PARSER_ERROR_TYPE_EXPECTED ?
                     error->pos :
                     handler->n_tokens - 1;

    char highlighted_token[1024] = { 0 };
    get_highlighted_token(handler, &handler->tokens[token_pos], ESCCOLOR_RED,
                          ESCGRAPHICS_BOLD, highlighted_token, 1023, hint);

    fprintf(stderr, "%s%s\n%s", msg_start, error_reason, highlighted_token);
  }

  return false;
}

static sz get_ulen(u32 n)
{
  sz len = 0;
  if (n == 0)
    return 1;

  while (n > 0) {
    n /= 10;
    len++;
  }

  return len;
}

static inline void get_message_start(const char *filename, sz line, sz start,
                                     const char *msg, esccolor_t esccolor,
                                     escgraphics_t escgraphics, char *out, sz n)
{
  snprintf(out, n, ESCGRAPHICS_BOLD "%s:%zu:%zu: %s%s%s: " ESC_RESET, filename,
           line + 1, start, escgraphics, esccolor, msg);
}

static void get_highlighted_token(error_handler_t *handler,
                                  const solc_token_t *tok, esccolor_t esccolor,
                                  escgraphics_t escgraphics, char *out, sz n,
                                  const char *msg_after)
{
  const sz line_num = tok->line + 1;
  const sz line_num_len = get_ulen(line_num);
  char line_buf[1080] = { 0 };
  get_line(handler->src, handler->src_len, tok->line, line_buf, 1023);

  insert_at(line_buf, ESC_RESET, tok->end + 1);
  insert_at(line_buf, esccolor, tok->end - tok->len + 1);
  insert_at(line_buf, escgraphics, tok->end - tok->len + 1);

  char mark[1024] = { 0 };
  char *pmark = &mark[0];
  *pmark++ = '^';
  for (sz i = 0; i < tok->len - 1 && i < 1024; i++)
    *pmark++ = '~';

  char *fmtbuf = calloc(n, sizeof(char));
  snprintf(fmtbuf, n,
           "  %%%zuzu | %%s\n"
           "  %%%zus | %s%s%%%zus %%s" ESC_RESET "\n",
           line_num_len, line_num_len, esccolor, escgraphics, tok->end + 1);
  snprintf(out, n, fmtbuf, line_num, line_buf, "", mark,
           msg_after ? msg_after : "");
  free(fmtbuf);
}

static void get_line(const char *src, sz src_len, sz line, char *out, sz n)
{
  sz pos = 0;

  while (pos < src_len && line > 0)
    if (src[pos++] == '\n')
      line--;

  while (pos < src_len) {
    char c = src[pos++];
    if (c == '\n')
      break;

    if (!n--)
      break;

    *out++ = c;
  }
}

static void get_parser_error_reason(const solc_parser_error_t *error,
                                    const solc_token_t *tokens, char *out, sz n)
{
  switch (error->type) {
  case SOLC_PARSER_ERROR_TYPE_UNK: {
    snprintf(out, n, "<Unknown error>");
  } break;

  case SOLC_PARSER_ERROR_TYPE_EXPECTED: {
    snprintf(out, n, "unexpected end of file");
  } break;

  case SOLC_PARSER_ERROR_TYPE_UNEXPECTED: {
    snprintf(out, n, "unexpected token \"" ESCGRAPHICS_BOLD "%s" ESC_RESET "\"",
             token_to_value(&tokens[error->pos]));
  } break;

  case SOLC_PARSER_ERROR_TYPE_UNEXPECTED_WHITESPACE: {
    snprintf(out, n,
             "unexpected whitespace after \"" ESCGRAPHICS_BOLD "%s" ESC_RESET
             "\"",
             token_to_value(&tokens[error->pos]));
  } break;

  case SOLC_PARSER_ERROR_TYPE_NO_CHAR_AFTER_BACKSLASH: {
    snprintf(out, n, "no character after backslash");
  } break;

  case SOLC_PARSER_ERROR_TYPE_UNKNOWN_SPECIAL_CHARACTER: {
    snprintf(out, n, "invalid special character in string or symbol");
  } break;

  case SOLC_PARSER_ERROR_TYPE_EXPR_EMPTY: {
    snprintf(out, n, "expression is empty");
  } break;

  case SOLC_PARSER_ERROR_TYPE_EXPR_NO_OPERAND_AFTER_PREFIX: {
    snprintf(out, n, "no operand after prefix operator");
  } break;

  case SOLC_PARSER_ERROR_TYPE_EXPR_PREFIX_AFTER_OPERAND: {
    snprintf(out, n, "prefix operator goes after an operand");
  } break;

  case SOLC_PARSER_ERROR_TYPE_EXPR_NON_PREFIX_OPERATOR_AT_THE_BEGINNING: {
    snprintf(out, n, "non-prefix operator at the beginning of the expression");
  } break;

  case SOLC_PARSER_ERROR_TYPE_EXPR_2_NON_PREFIX_OPERATORS: {
    snprintf(out, n, "two non-prefix operators");
  } break;

  case SOLC_PARSER_ERROR_TYPE_EXPR_2_OPERANDS: {
    snprintf(out, n, "two operands");
  } break;

  case SOLC_PARSER_ERROR_TYPE_EXPR_2_ASSIGN_OPERATORS: {
    snprintf(out, n, "two assign operators");
  } break;

  case SOLC_PARSER_ERROR_TYPE_EXPR_LAST_NODE_IS_NOT_AN_OPERAND: {
    snprintf(out, n, "last node in expression is not an operand");
  } break;

  default:
    SOLC_NOREACH();
  }
}

static const char *token_display(solc_tokentype_t type)
{
  switch (type) {
  case SOLC_TOKENTYPE_ERR:
    return "<ERROR>";

  case SOLC_TOKENTYPE_ID:
    return "an identifier";

  case SOLC_TOKENTYPE_NUM:
  case SOLC_TOKENTYPE_NUMHEX:
  case SOLC_TOKENTYPE_NUMBIN:
  case SOLC_TOKENTYPE_NUMOCT:
  case SOLC_TOKENTYPE_NUMFLOAT:
    return "a number";

  case SOLC_TOKENTYPE_STRING:
    return "a string";

  case SOLC_TOKENTYPE_SYMBOL:
    return "a character";

  case SOLC_TOKENTYPE_LPAREN:
    return "(";
  case SOLC_TOKENTYPE_RPAREN:
    return ")";
  case SOLC_TOKENTYPE_LBRACK:
    return "[";
  case SOLC_TOKENTYPE_RBRACK:
    return "]";
  case SOLC_TOKENTYPE_LCBRACK:
    return "{";
  case SOLC_TOKENTYPE_RCBRACK:
    return "}";

  case SOLC_TOKENTYPE_LARROW:
    return "<";
  case SOLC_TOKENTYPE_RARROW:
    return ">";

  case SOLC_TOKENTYPE_COLON:
    return ":";
  case SOLC_TOKENTYPE_SEMI:
    return ";";

  case SOLC_TOKENTYPE_COMMA:
    return ",";
  case SOLC_TOKENTYPE_PERIOD:
    return ".";

  case SOLC_TOKENTYPE_AMPERSAND:
    return "&";
  case SOLC_TOKENTYPE_PIPE:
    return "|";
  case SOLC_TOKENTYPE_CIRCUMFLEX:
    return "^";
  case SOLC_TOKENTYPE_TILDE:
    return "~";

  case SOLC_TOKENTYPE_EXCLMARK:
    return "!";

  case SOLC_TOKENTYPE_PLUS:
    return "+";
  case SOLC_TOKENTYPE_MINUS:
    return "-";
  case SOLC_TOKENTYPE_ASTERISK:
    return "*";
  case SOLC_TOKENTYPE_SLASH:
    return "/";
  case SOLC_TOKENTYPE_PERCENT:
    return "%";

  case SOLC_TOKENTYPE_EQ:
    return "=";

  case SOLC_TOKENTYPE_AT:
    return "@";
  case SOLC_TOKENTYPE_HASH:
    return "#";

  default:
    SOLC_NOREACH();
  }
}

static const char *token_to_value(const solc_token_t *token)
{
  switch (token->type) {
  case SOLC_TOKENTYPE_ID:
  case SOLC_TOKENTYPE_NUM:
  case SOLC_TOKENTYPE_NUMHEX:
  case SOLC_TOKENTYPE_NUMBIN:
  case SOLC_TOKENTYPE_NUMOCT:
  case SOLC_TOKENTYPE_NUMFLOAT:
  case SOLC_TOKENTYPE_STRING:
  case SOLC_TOKENTYPE_SYMBOL:
    return token->value;

  default:
    return token_display(token->type);
  }
}

static void insert_at(char *dst, const char *src, sz i)
{
  for (; *dst && i; dst++, i--)
    ;

  sz src_len = strlen(src);
  sz dst_len = strlen(dst);
  memmove(dst + src_len, dst, dst_len);
  memcpy(dst, src, src_len);
}
