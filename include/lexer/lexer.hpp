#pragma once

#include <cstddef>
#include <lexer/token.hpp>
#include <string>
#include <vector>

namespace solc
{

class Lexer {
  public:
  Lexer() = default;

  std::vector<Token> tokenize(const std::string &src);

  private:
  std::string _src{};
  size_t _line{};
  size_t _llp{};
  size_t _pos{};

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

  Token gen_token(size_t len, size_t end, TokenType type,
                  const std::string &value = "") const;

  char peek(size_t pos) const;

  bool is_processable(char c) const;
};

}
