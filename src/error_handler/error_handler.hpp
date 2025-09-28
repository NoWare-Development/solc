#pragma once

#include "types.hpp"
#include "util/util.hpp"
#include <lexer/token.hpp>
#include <memory>
#include <parser/parser.hpp>
#include <sa/sa.hpp>
#include <vector>

class ErrorHandler
{
public:
  ErrorHandler (const std::string &filename, const std::string &src);

  void add_tokens (const std::vector<nlc::Token> &tokens);
  void add_parser_errors (const std::vector<nlc::Parser::ParserError> &errors);

  bool handle_tokens () const;
  bool handle_parser_errors () const;
  bool handle_invalid_expressions (const nlc::AST &root) const;
  void handle_sa_errors (
      const std::vector<nlc::SemanticAnalyzer::SAError> &errors) const;

  std::string
  get_sa_error_reason (const nlc::SemanticAnalyzer::SAError &err) const;

private:
  std::vector<nlc::Token> _tokens{};
  std::vector<nlc::Parser::ParserError> _parser_errors{};
  size_t _nol{ 1 };
  size_t _numoffset{};

  std::string _filename;
  std::string _src;

  void print_errored_tokens () const;
  void print_parser_errors () const;

  void collect_invalid_expressions (const nlc::AST &node,
                                    std::vector<nlc::AST> &v) const;

  std::string get_token_error (const nlc::Token &tok) const;
  std::string get_parser_error (const nlc::Parser::ParserError &err) const;

  std::string
  get_parser_error_reason (const nlc::Parser::ParserError &err) const;

  std::string get_message_start (size_t line, size_t start,
                                 const std::string &msg, ESCColor color,
                                 ESCGraphics mode
                                 = ESCGraphics::ESCGRAPHICS_RESET) const;

  std::string get_highlighted_token (const nlc::Token &tok, ESCColor color,
                                     ESCGraphics mode
                                     = ESCGraphics::ESCGRAPHICS_RESET) const;

  bool has_invalid_tokens () const;
  bool has_parser_errors () const;

  std::string get_line (size_t linenum) const;
  std::string last_line () const;

  size_t get_number_length (long long num) const;

  std::string type_to_string (std::shared_ptr<nlc::Type> type) const;
};
