#include "solc/lexer/lexer.h"
#include "solc/lexer/token.h"
#include <ctype.h>
#include <containers/vector.h>
#include <string.h>
#include "global.h"
#include "allocs/alloc_arena.h"

static inline solc_token_t process_id(solc_lexer_t *lexer);
static inline solc_token_t process_num(solc_lexer_t *lexer);
static inline solc_token_t process_numhex(solc_lexer_t *lexer);
static inline solc_token_t process_numbin(solc_lexer_t *lexer);
static inline solc_token_t process_numoct(solc_lexer_t *lexer);
static inline solc_token_t process_string(solc_lexer_t *lexer);
static inline solc_token_t process_symbol(solc_lexer_t *lexer);
static inline solc_token_t process_err(solc_lexer_t *lexer);
static inline void skip_comments(solc_lexer_t *lexer);

static inline b8 is_start_of_id(char c);
static inline b8 is_char_of_id(char c);

static inline solc_token_t gen_token(solc_lexer_t *lexer, sz len, sz end,
                                     solc_tokentype_t type, char *value);

static inline char peek(solc_lexer_t *lexer, sz pos);

static inline b8 is_processable(char c);

solc_lexer_t *solc_lexer_create(const char *src)
{
  solc_lexer_t *lexer =
    alloc_arena_allocate(global_arena_alloc(), sizeof(solc_lexer_t));
  memset(lexer, 0, sizeof(solc_lexer_t));
  lexer->tokens_v = vector_reserve(solc_token_t, 1024);
  lexer->src = src;
  lexer->src_len = strlen(src);

  return lexer;
}

void solc_lexer_destroy(solc_lexer_t *lexer)
{
  if (lexer == nullptr)
    return;

  vector_destroy(lexer->tokens_v);

  memset(lexer, -1, sizeof(solc_lexer_t));
}

solc_token_t *solc_lexer_tokenize(solc_lexer_t *lexer, sz *out_token_num)
{
  lexer->pos = 0;
  lexer->line = 0;
  lexer->llp = 0;

  while (lexer->pos < lexer->src_len) {
    char c = lexer->src[lexer->pos];

    // Skip whitespaces
    if (isspace(c)) {
      lexer->pos++;
      if (c == '\n') {
        // If current character is a newline character,
        // increment current line number by 1 and set last line position.
        lexer->line++;
        lexer->llp = lexer->pos;
      }
      continue;
    }

    // Check for invalid symbols
    if (!is_processable(c)) {
      vector_push(lexer->tokens_v, process_err(lexer));
      continue;
    }

    // Check for identifiers
    if (is_start_of_id(c)) {
      vector_push(lexer->tokens_v, process_id(lexer));
      continue;
    }

    // Check for numbers
    if (isdigit(c)) {
      vector_push(lexer->tokens_v, process_num(lexer));
      continue;
    }

    switch (c) {
    case '"': {
      vector_push(lexer->tokens_v, process_string(lexer));
      continue;
    }

    case '\'': {
      vector_push(lexer->tokens_v, process_symbol(lexer));
      continue;
    }

    case '(': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_LPAREN, nullptr));
    } break;
    case ')': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_RPAREN, nullptr));
    } break;

    case '[': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_LBRACK, nullptr));
    } break;
    case ']': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_RBRACK, nullptr));
    } break;

    case '{': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_LCBRACK, nullptr));
    } break;
    case '}': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_RCBRACK, nullptr));
    } break;

    case '<': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_LARROW, nullptr));
    } break;
    case '>': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_RARROW, nullptr));
    } break;

    case '+': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_PLUS, nullptr));
    } break;
    case '-': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_MINUS, nullptr));
    } break;
    case '*': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_ASTERISK, nullptr));
    } break;
    case '/': {
      char next = peek(lexer, lexer->pos + 1);
      if (next == '/' || next == '*') {
        skip_comments(lexer);
        continue;
      }

      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_SLASH, nullptr));
    } break;
    case '%': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_PERCENT, nullptr));
    } break;

    case '&': {
      vector_push(lexer->tokens_v,
                  gen_token(lexer, 1, lexer->pos, SOLC_TOKENTYPE_AMPERSAND,
                            nullptr));
    } break;
    case '|': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_PIPE, nullptr));
    } break;
    case '~': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_TILDE, nullptr));
    } break;
    case '^': {
      vector_push(lexer->tokens_v,
                  gen_token(lexer, 1, lexer->pos, SOLC_TOKENTYPE_CIRCUMFLEX,
                            nullptr));
    } break;

    case '=': {
      vector_push(lexer->tokens_v,
                  gen_token(lexer, 1, lexer->pos, SOLC_TOKENTYPE_EQ, nullptr));
    } break;

    case '!': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_EXCLMARK, nullptr));
    } break;

    case '@': {
      vector_push(lexer->tokens_v,
                  gen_token(lexer, 1, lexer->pos, SOLC_TOKENTYPE_AT, nullptr));
    } break;

    case ':': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_COLON, nullptr));
    } break;
    case ';': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_SEMI, nullptr));
    } break;

    case '#': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_HASH, nullptr));
    } break;

    case '.': {
      if (isdigit(peek(lexer, lexer->pos + 1))) {
        vector_push(lexer->tokens_v, process_num(lexer));
        continue;
      }
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_PERIOD, nullptr));
    } break;
    case ',': {
      vector_push(lexer->tokens_v, gen_token(lexer, 1, lexer->pos,
                                             SOLC_TOKENTYPE_COMMA, nullptr));
    } break;

    default:
      break;
    }

    lexer->pos++;
  }

  *out_token_num = vector_get_length(lexer->tokens_v);
  sz lexersize = *out_token_num * sizeof(solc_token_t);
  solc_token_t *out_tokens =
    alloc_arena_allocate(global_arena_alloc(), lexersize);
  memcpy(out_tokens, lexer->tokens_v, lexersize);
  vector_clear(lexer->tokens_v);
  return out_tokens;
}

// Consume all symbols that are valid for identifiers
// and put them into an identifier token.
static inline solc_token_t process_id(solc_lexer_t *lexer)
{
  sz start = lexer->pos;

  for (; lexer->pos < lexer->src_len; lexer->pos++)
    if (!is_char_of_id(lexer->src[lexer->pos]))
      break;

  sz len = lexer->pos - start;

  char *id_value =
    alloc_arena_allocate(global_arena_alloc(), sizeof(char) * (len + 1));
  memcpy(id_value, &lexer->src[start], len);
  return gen_token(lexer, len, lexer->pos - 1, SOLC_TOKENTYPE_ID, id_value);
}

// Consume all digit symbols
// and put them into a number token (dec, hex, oct, bin, float).
static inline solc_token_t process_num(solc_lexer_t *lexer)
{
  char c = lexer->src[lexer->pos];
  if (c == '0') {
    char c2 = peek(lexer, lexer->pos + 1);
    switch (c2) {
    case 'x':
      return process_numhex(lexer);
    case 'b':
      return process_numbin(lexer);
    default: {
      if (isdigit(c2)) {
        return process_numoct(lexer);
      } else if (c2 != '.') {
        lexer->pos++;
        char *zero_str = alloc_arena_allocate(global_arena_alloc(), 2);
        zero_str[0] = '0';
        zero_str[1] = 0;
        return gen_token(lexer, 1, lexer->pos - 1, SOLC_TOKENTYPE_NUM,
                         zero_str);
      }
      break;
    }
    }
  }

  b8 has_dot = false;
  sz start = lexer->pos;

  while (lexer->pos < lexer->src_len) {
    char c = lexer->src[lexer->pos];
    if (!isdigit(c) && c != '.' && c != '\'' && c != '_')
      break;

    // ' or _ can be used as a separator inside numbers, so skip them.
    if (c == '\'' || c == '_') {
      lexer->pos++;
      continue;
    }

    if (c == '.') {
      if (has_dot)
        break;

      has_dot = true;
    }
    lexer->pos++;
  }

  // But all preprocessed lexer into a buffer.
  sz preproc_len = lexer->pos - start;
  char *preproc_buf = alloc_arena_allocate(global_arena_alloc(),
                                           sizeof(char) * (preproc_len + 1));
  memcpy(preproc_buf, &lexer->src[start], preproc_len);

  // Put only real numbers (and dots) into an output array.
  // We will probably allocate a bit more than we need, but it is fine I guess.
  b8 first_is_period = preproc_buf[0] == '.';
  char *out_value = alloc_arena_allocate(
    global_arena_alloc(), sizeof(char) * (preproc_len + 1 + first_is_period));
  if (first_is_period)
    out_value[0] = '0';

  for (char *ppbuf = preproc_buf, *ov = out_value + first_is_period; *ppbuf;
       ppbuf++) {
    if (*ppbuf != '\'' && *ppbuf != '_')
      *ov++ = *ppbuf;
  }

  return gen_token(lexer, lexer->pos - start, lexer->pos - 1,
                   has_dot ? SOLC_TOKENTYPE_NUMFLOAT : SOLC_TOKENTYPE_NUM,
                   out_value);
}

static inline solc_token_t process_numhex(solc_lexer_t *lexer)
{
  lexer->pos += 2;

  // TODO: we probably should take into account that there may be number
  // separators, but since it is an initial rewrite, I decided to leave it
  // for now.

  sz start = lexer->pos;
  for (; lexer->pos < lexer->src_len; lexer->pos++)
    if (!isxdigit(lexer->src[lexer->pos]))
      break;

  sz len = lexer->pos - start;
  char *out_value =
    alloc_arena_allocate(global_arena_alloc(), sizeof(char) * (len + 1));
  memcpy(out_value, &lexer->src[start], len);

  return gen_token(lexer, len + 2, lexer->pos - 1, SOLC_TOKENTYPE_NUMHEX,
                   out_value);
}

static inline solc_token_t process_numbin(solc_lexer_t *lexer)
{
  lexer->pos += 2;

  // TODO: we probably should take into account that there may be number
  // separators, but since it is an initial rewrite, I decided to leave it
  // for now.

  sz start = lexer->pos;
  for (; lexer->pos < lexer->src_len; lexer->pos++) {
    char c = lexer->src[lexer->pos];
    if (c != '0' && c != '1')
      break;
  }

  sz len = lexer->pos - start;
  char *out_value =
    alloc_arena_allocate(global_arena_alloc(), sizeof(char) * (len + 1));
  memcpy(out_value, &lexer->src[start], len);

  return gen_token(lexer, len + 2, lexer->pos - 1, SOLC_TOKENTYPE_NUMBIN,
                   out_value);
}

static inline solc_token_t process_numoct(solc_lexer_t *lexer)
{
  lexer->pos++;

  // TODO: we probably should take into account that there may be number
  // separators, but since it is an initial rewrite, I decided to leave it
  // for now.

  sz start = lexer->pos;
  for (; lexer->pos < lexer->src_len; lexer->pos++) {
    char c = lexer->src[lexer->pos];
    if (c < '0' || c > '7')
      break;
  }

  sz len = lexer->pos - start;
  char *out_value =
    alloc_arena_allocate(global_arena_alloc(), sizeof(char) * (len + 1));
  memcpy(out_value, &lexer->src[start], len);

  return gen_token(lexer, len + 1, lexer->pos - 1, SOLC_TOKENTYPE_NUMOCT,
                   out_value);
}

static inline solc_token_t process_string(solc_lexer_t *lexer)
{
  lexer->pos++;

  sz start = lexer->pos;
  for (; lexer->pos < lexer->src_len; lexer->pos++) {
    char c = lexer->src[lexer->pos];
    if (c == '\"') {
      lexer->pos++;
      break;
    }

    if (c == '\n') {
      lexer->line++;
      lexer->llp = lexer->pos + 1;
    }
  }

  sz len = lexer->pos - start - 1;
  char *out_value =
    alloc_arena_allocate(global_arena_alloc(), sizeof(char) * (len + 1));
  memcpy(out_value, &lexer->src[start], len);

  return gen_token(lexer, len + 2, lexer->pos - 1, SOLC_TOKENTYPE_STRING,
                   out_value);
}

static inline solc_token_t process_symbol(solc_lexer_t *lexer)
{
  lexer->pos++;

  sz start = lexer->pos;
  for (; lexer->pos < lexer->src_len; lexer->pos++) {
    char c = lexer->src[lexer->pos];
    if (c == '\'') {
      lexer->pos++;
      break;
    }

    if (c == '\n') {
      lexer->line++;
      lexer->llp = lexer->pos + 1;
    }
  }

  sz len = lexer->pos - start - 1;
  char *out_value =
    alloc_arena_allocate(global_arena_alloc(), sizeof(char) * (len + 1));
  memcpy(out_value, &lexer->src[start], len);

  return gen_token(lexer, len + 2, lexer->pos - 1, SOLC_TOKENTYPE_SYMBOL,
                   out_value);
}

static inline solc_token_t process_err(solc_lexer_t *lexer)
{
  sz err_start = lexer->pos;
  for (; lexer->pos < lexer->src_len; lexer->pos++)
    if (is_processable(lexer->src[lexer->pos]))
      break;
  sz len = lexer->pos - err_start;
  char *err_value =
    alloc_arena_allocate(global_arena_alloc(), sizeof(char) * (len + 1));
  memcpy(err_value, &lexer->src[err_start], len);
  return gen_token(lexer, len, lexer->pos - 1, SOLC_TOKENTYPE_ERR, err_value);
}

static inline void skip_comments(solc_lexer_t *lexer)
{
  if (lexer->pos + 1 >= lexer->src_len)
    return;

  char c1 = lexer->src[lexer->pos];
  char c2 = lexer->src[lexer->pos + 1];

  if (c1 == c2 && c2 == '/') { // C++ comments
    while (lexer->pos < lexer->src_len) {
      char c = lexer->src[lexer->pos];
      if (c == '\n') {
        lexer->line++;
        lexer->pos++;
        lexer->llp = lexer->pos;
        break;
      }
      lexer->pos++;
    }
  } else if (c1 == '/' && c2 == '*') { // C comments
    lexer->pos += 2;
    while (lexer->pos < lexer->src_len) {
      char c = lexer->src[lexer->pos];
      if (c == '\n') {
        lexer->line++;
        lexer->pos++;
        lexer->llp = lexer->pos;
        continue;
      }

      if (lexer->pos + 1 < lexer->src_len) {
        char c2 = lexer->src[lexer->pos + 1];
        if (c == '*' && c2 == '/') {
          lexer->pos += 2;
          break;
        }
      }

      lexer->pos++;
    }
  }
}

static inline b8 is_start_of_id(char c)
{
  return isalpha(c) || c == '$' || c == '_';
}

static inline b8 is_char_of_id(char c)
{
  return is_start_of_id(c) || isdigit(c);
}

static inline solc_token_t gen_token(solc_lexer_t *lexer, sz len, sz end,
                                     solc_tokentype_t type, char *value)
{
  b8 has_whitespace_after = end + 1 >= lexer->src_len ||
                            isspace(lexer->src[end + 1]);
  return (solc_token_t){ value, lexer->line, end - lexer->llp,
                         len,   type,        has_whitespace_after };
}

static inline char peek(solc_lexer_t *lexer, sz pos)
{
  if (pos < lexer->src_len) {
    return lexer->src[pos];
  }
  return 0;
}

static inline b8 is_processable(char c)
{
  return is_start_of_id(c) || isdigit(c) || c == '.' || c == '!' || c == '?' ||
         c == '*' || c == '/' || c == '+' || c == '-' || c == '^' || c == '&' ||
         c == '|' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' ||
         c == '}' || c == '"' || c == '\'' || c == '<' || c == '>' ||
         c == '=' || c == '%' || c == '@' || c == ':' || c == ';' || c == '#' ||
         c == ',' || c == '~';
}
