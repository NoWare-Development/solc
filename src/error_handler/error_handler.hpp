#pragma once

#include "util/util.hpp"
#include <lexer/token.hpp>
//#include <parser/parser.hpp>
#include <vector>

class ErrorHandler {
  public:
  ErrorHandler(const std::string &filename, const std::string &src);

  void add_tokens(const std::vector<solc::Token> &tokens);
  //void add_parser_errors(const std::vector<solc::Parser::ParserError> &errors);

  bool handle_tokens() const;
  bool handle_parser_errors() const;
  //bool handle_invalid_expressions(const solc::AST &root) const;

  private:
  std::vector<solc::Token> _tokens{};
  //std::vector<solc::Parser::ParserError> _parser_errors{};
  size_t _nol{ 1 };
  size_t _numoffset{};

  std::string _filename;
  std::string _src;

  void print_errored_tokens() const;
  void print_parser_errors() const;

  //void collect_invalid_expressions(const solc::AST &node,
  //                                 std::vector<solc::AST> &v) const;

  std::string get_token_error(const solc::Token &tok) const;
  //std::string get_parser_error(const solc::Parser::ParserError &err) const;

  //std::string
  //get_parser_error_reason(const solc::Parser::ParserError &err) const;

  std::string
  get_message_start(size_t line, size_t start, const std::string &msg,
                    ESCColor color,
                    ESCGraphics mode = ESCGraphics::ESCGRAPHICS_RESET) const;

  std::string get_highlighted_token(
    const solc::Token &tok, ESCColor color,
    ESCGraphics mode = ESCGraphics::ESCGRAPHICS_RESET) const;

  bool has_invalid_tokens() const;
  bool has_parser_errors() const;

  std::string get_line(size_t linenum) const;
  std::string last_line() const;

  size_t get_number_length(long long num) const;
};
