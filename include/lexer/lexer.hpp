#pragma once

#include <cstddef>
#include <lexer/token.hpp>
#include <string>
#include <vector>

namespace solc
{

// Lexer class is used to convert sources into a list of tokens.
class Lexer {
  public:
  Lexer() = default;

  // Turn sources into a list of tokens.
  std::vector<Token> tokenize(const std::string &src);

  private:
  std::string _src{}; // Sources
  size_t _line{}; // Current line
  size_t _llp{}; // Last line position
  size_t _pos{}; // Absolute position

  Token process_id();
  Token process_num();
  Token process_numhex();
  Token process_numbin();
  Token process_numoct();
  Token process_string();
  Token process_symbol();
  Token process_err();

  void skip_comments();

  bool is_start_of_id(char c) const;
  bool is_char_of_id(char c) const;

  // Generates token.
  Token gen_token(size_t len, size_t end, TokenType type,
                  const std::string &value = "") const;

  // Peeks into a position in sources.
  char peek(size_t pos) const;

  // Checks if symbol is "processable".
  bool is_processable(char c) const;
};

}
