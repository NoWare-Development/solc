#pragma once

#include <cstdint>
#include <lexer/token.hpp>
#include <map>
#include <parser/ast.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace solc
{

class Parser {
  public:
  Parser(const std::vector<Token> &tokens)
    : _tokens{ tokens }
  {
  }

  struct ParserError {
    enum class Type {
      UNK,
      EXPECTED,
      UNEXPECTED,
      INVALID_EXPR,
    };

    Type type;
    size_t pos;
    size_t len;

    ParserError(Type type, size_t pos, size_t len = 1)
      : type{ type }
      , pos{ pos }
      , len{ len }
    {
    }
  };

  AST parse();

  const std::vector<ParserError> &get_errors() const
  {
    return _errors;
  }

  private:
  std::vector<Token> _tokens;
  std::vector<ParserError> _errors{};
  size_t _pos{};
  bool _errored = false;

  // <top(export)>
  //   : <decldef>
  //   | <typedef>
  //   | <structdef>
  //   | <enumdef>
  //   | <uniondef>
  //   | <import>
  //   | <export>
  //   | <extern>
  //   ;
  AST parse_top();

  // <typedef>
  //   : typedef <type> <id>;
  //   ;
  AST parse_typedef();

  // <type>
  //   : <typeraw>
  //   | <typeraw>[<expr>]
  //   ;
  AST parse_type();

  // <typeraw>
  //   : *<typeraw>
  //   | <id>
  //   | <funcptrtype>
  //   ;
  AST parse_type_raw();

  // <funcptrtype>
  //   : (<funcargs>) -> <type>
  //   ;
  AST parse_function_pointer_type();

  // <enumdef>
  //   : enum <id> { <enumelements> }
  //   ;
  AST parse_enum_definition();
  AST parse_enum_element();

  // <import>
  //   : import <module>;
  //   ;
  AST parse_import();

  // <export>
  //   : export <top(export)>
  //   ;
  AST parse_export();

  // <extern>
  //   : extern <funcproto>;
  //   ;
  AST parse_extern();

  // <module>
  //   : <id>.<module>
  //   | <id>
  //   ;
  AST parse_module();

  // <decldef>
  //   : <modifier> <decldef>
  //   | <vardecl>;
  //   | <vardef>;
  //   | <funcdef>
  //   ;
  AST parse_decldef();

  // <vardecl>
  //   : <id>: <type>
  //   ;
  //
  // <vardef>
  //   : <id>: <type> = <expr>
  //   ;
  AST parse_variable_decldef();

  // <funcdef>
  //   : <id> :: (<funcargs>) [-> <type>] <stmtlist>
  //   ;
  AST parse_function();
  // <generic funcdef>
  //   : <id> <placeholder types> (<funcargs>) [-> <type>] <stmtlist>
  AST parse_generic_function();

  // <funcarg>
  //   : <vardecl>
  //   | <vardef>
  //   | ...
  //   ;
  AST parse_function_argument();

  // <stmt>
  //   : ;
  //   | <exprstmt>
  //   | <returnstmt>
  //   | <gotostmt>
  //   | <labelstmt>
  //   | <decldef>
  //   | <whilestmt>
  //   | <ifstmt>
  //   | <forstmt>
  //   | <dowhilestmt>
  //   | <switchstmt>
  //   | <deferstmt>
  //   | <structdef>
  //   | <enumdef>
  //   | <uniondef>
  //   | break;
  //   | continue;
  //   ;
  AST parse_statement();
  AST parse_expression_statement(); // <exprstmt>: <expr>;

  AST parse_expression_statement_or_generic_func();

  // <stmtlist>
  //   : <stmt> <stmtlist>
  //   | <stmt>
  //   ;
  AST parse_statement_list();

  // <returnstmt>
  //   : return <expr>;
  //   ;
  AST parse_return_statement();

  // <gotostmt>
  //   : goto <label>;
  //   ;
  AST parse_goto_statement();

  // <labelstmt>
  //   : <label>:
  //   ;
  AST parse_label_statement();

  // <label>
  //   : @<id>
  //   ;
  AST parse_label();

  // <whilestmt>
  //   : while (<expr>) <stmt>
  //   ;
  AST parse_while_statement();

  // <forstmt>
  //   : for (<stmt> <expr>; <expr>)
  //   ;
  AST parse_for_statement();

  // <dowhilestmt>
  //   : do <stmt> while (<expr>);
  //   ;
  AST parse_do_while_statement();

  // <switchstmt>
  //   : switch (<expr>) { <casestmts> }
  //   ;
  AST parse_switch_statement();

  // <casestmt>
  //   : case <expr>: <stmt>
  //   ;
  AST parse_case_statement();

  // <deferstmt>
  //   : defer <stmt>
  //   ;
  AST parse_defer_statement();

  // <ifstmt>
  //   : if <stmt> <elsestmt>
  //   | if <stmt>
  //   ;
  AST parse_if_statement();

  // <elsestmt>
  //   : else <stmt>
  //   ;
  AST parse_else_statement();

  // <structdef>
  //   : struct <id> [< <generic types> >] { <decldefs> }
  //   ;
  AST parse_struct();

  // <uniondef>
  //   : union <id> { <decldefs> }
  //   ;
  AST parse_union();

  // <vismarker>
  //   : public:
  //   | private:
  //   ;
  AST parse_visibility_marker();

  // <initlist>
  //   : { <initlistentries> }
  //   ;
  AST parse_initialization_list();

  // <initlistentry>
  //   : <expr>
  //   | .<id> = <expr>
  //   ;
  AST parse_initialization_list_entry();

  // <arglist>
  //   : (<args>)
  //   ;
  // <arg>
  //   : <modifier> <arg>
  //   | <id>: <type>
  //   ;
  AST parse_argument_list();

  AST parse_break_statement(); // break;
  AST parse_continue_statement(); // continue;

  AST parse_generic_placeholder_type_list(); // < <placeholder types> >
  AST parse_generic_type_list(); // < <types> >

  // <expr>
  // NOTE: `toplevel` is passed only in expression statements.
  // It allows to use assignment operators in the expression.
  AST parse_expression(bool toplevel = false);
  AST parse_expression_tree(bool toplevel = false);
  AST parse_expression_operand();
  AST parse_call_operand();
  bool is_generic_call_operand();
  AST parse_generic_call_operand();
  AST parse_identifier_operand(bool accept_modules = true,
                               bool accept_functions = true);
  AST parse_array_element(AST array);
  AST pratt_parse_expression(const std::vector<AST> &in, size_t *pos,
                             int min_bp) const;
  bool validate_expression(const AST &expr_ast, size_t &invalid_pos,
                           std::string &out_reason) const;
  void get_binding_power(ASTType op_type, int &l_bp, int &r_bp) const;

  AST number_to_operand(Token &tok) const;
  uint64_t string_to_dec(const std::string &str) const;
  uint64_t string_to_hex(const std::string &str) const;
  uint64_t string_to_bin(const std::string &str) const;
  uint64_t string_to_oct(const std::string &str) const;
  std::string hex_to_string(uint64_t val) const;

  void skip_until(TokenType type);

  bool verify_pos(size_t pos);
  bool verify_token(size_t pos, TokenType got, TokenType expected);
  bool verify_value(size_t pos, const std::string &got,
                    const std::string &expected);

  TokenType peek(size_t pos) const;

  bool is_modifier(const std::string &str) const;

  bool is_operator(TokenType type) const;
  bool is_binary_operator(TokenType type) const;
  bool is_assign_operator(TokenType type) const;
  bool is_compare_operator(TokenType type) const;
  bool is_boolean_operator(TokenType type) const;
  bool is_prefix_operator(TokenType type) const;
  bool is_numeric_token(TokenType type) const;

  bool is_operator(ASTType type) const;
  bool is_binary_operator(ASTType type) const;
  bool is_assign_operator(ASTType type) const;
  bool is_compare_operator(ASTType type) const;
  bool is_boolean_operator(ASTType type) const;
  bool is_prefix_operator(ASTType type) const;

  bool is_operand(ASTType type) const;

  void advance_to_terminator();

  template <typename... Args> void add_error(Args &&...args)
  {
    _errored = true;
    _errors.emplace_back(args...);
    advance_to_terminator();
  }

  const std::map<TokenType, ASTType> _binary_operators = {
    { TokenType::ADD, ASTType::EXPR_BINARY_OPERATOR_ADD },
    { TokenType::SUB, ASTType::EXPR_BINARY_OPERATOR_SUB },
    { TokenType::MUL, ASTType::EXPR_BINARY_OPERATOR_MUL },
    { TokenType::DIV, ASTType::EXPR_BINARY_OPERATOR_DIV },
    { TokenType::MOD, ASTType::EXPR_BINARY_OPERATOR_MOD },
    { TokenType::SHL, ASTType::EXPR_BINARY_OPERATOR_SHL },
    { TokenType::SHR, ASTType::EXPR_BINARY_OPERATOR_SHR },
    { TokenType::BAND, ASTType::EXPR_BINARY_OPERATOR_AND },
    { TokenType::BOR, ASTType::EXPR_BINARY_OPERATOR_OR },
    { TokenType::BXOR, ASTType::EXPR_BINARY_OPERATOR_XOR },
  };

  const std::map<TokenType, ASTType> _compare_operators = {
    { TokenType::EQEQ, ASTType::EXPR_COMPARE_OPERATOR_EQ },
    { TokenType::NOTEQ, ASTType::EXPR_COMPARE_OPERATOR_NOTEQ },
    { TokenType::LTHAN, ASTType::EXPR_COMPARE_OPERATOR_LTHAN },
    { TokenType::GTHAN, ASTType::EXPR_COMPARE_OPERATOR_GTHAN },
    { TokenType::LTHANEQ, ASTType::EXPR_COMPARE_OPERATOR_LTHANEQ },
    { TokenType::GTHANEQ, ASTType::EXPR_COMPARE_OPERATOR_GTHANEQ },
  };

  const std::map<TokenType, ASTType> _boolean_operators = {
    { TokenType::AND, ASTType::EXPR_BOOLEAN_OPERATOR_AND },
    { TokenType::OR, ASTType::EXPR_BOOLEAN_OPERATOR_OR },
  };

  const std::map<TokenType, ASTType> _assign_operators = {
    { TokenType::EQ, ASTType::EXPR_ASSIGN_OPERATOR_EQ },
    { TokenType::ADDEQ, ASTType::EXPR_ASSIGN_OPERATOR_ADDEQ },
    { TokenType::SUBEQ, ASTType::EXPR_ASSIGN_OPERATOR_SUBEQ },
    { TokenType::MULEQ, ASTType::EXPR_ASSIGN_OPERATOR_MULEQ },
    { TokenType::DIVEQ, ASTType::EXPR_ASSIGN_OPERATOR_DIVEQ },
    { TokenType::MODEQ, ASTType::EXPR_ASSIGN_OPERATOR_MODEQ },
    { TokenType::SHLEQ, ASTType::EXPR_ASSIGN_OPERATOR_SHLEQ },
    { TokenType::SHREQ, ASTType::EXPR_ASSIGN_OPERATOR_SHREQ },
    { TokenType::BANDEQ, ASTType::EXPR_ASSIGN_OPERATOR_ANDEQ },
    { TokenType::BOREQ, ASTType::EXPR_ASSIGN_OPERATOR_OREQ },
    { TokenType::BXOREQ, ASTType::EXPR_ASSIGN_OPERATOR_XOREQ },
  };

  const std::map<TokenType, ASTType> _prefix_operators = {
    { TokenType::NOT, ASTType::EXPR_PREFIX_OPERATOR_NOT },
    { TokenType::BNOT, ASTType::EXPR_PREFIX_OPERATOR_BNOT },
    { TokenType::SUB, ASTType::EXPR_PREFIX_OPERATOR_NEG },
    { TokenType::MUL, ASTType::EXPR_PREFIX_OPERATOR_DEREF },
    { TokenType::BAND, ASTType::EXPR_PREFIX_OPERATOR_ADDRESS },
  };

  const std::unordered_set<TokenType> _numeric_tokens = {
    TokenType::NUM,    TokenType::NUMHEX,   TokenType::NUMBIN,
    TokenType::NUMOCT, TokenType::NUMFLOAT,
  };

  const std::unordered_set<std::string> _modifiers = {
    "const",
    "inline",
    "static",
  };

  const std::unordered_map<std::string, AST (Parser::*)()> _top_parse_methods = {
    { "enum", &Parser::parse_enum_definition },
    { "typedef", &Parser::parse_typedef },
    { "struct", &Parser::parse_struct },
    { "union", &Parser::parse_union },
    { "import", &Parser::parse_import },
    { "export", &Parser::parse_export },
    { "extern", &Parser::parse_extern },
  };

  const std::unordered_map<std::string, AST (Parser::*)()> _stmt_parse_methods = {
    { "struct", &Parser::parse_struct },
    { "union", &Parser::parse_union },
    { "enum", &Parser::parse_enum_definition },
    { "return", &Parser::parse_return_statement },
    { "goto", &Parser::parse_goto_statement },
    { "break", &Parser::parse_break_statement },
    { "continue", &Parser::parse_continue_statement },
    { "while", &Parser::parse_while_statement },
    { "for", &Parser::parse_for_statement },
    { "do", &Parser::parse_do_while_statement },
    { "switch", &Parser::parse_switch_statement },
    { "defer", &Parser::parse_defer_statement },
    { "if", &Parser::parse_if_statement },
    { "typedef", &Parser::parse_typedef },
  };

  const std::unordered_map<std::string, AST (Parser::*)()>
    _struct_parse_methods = {
      { "public", &Parser::parse_visibility_marker },
      { "private", &Parser::parse_visibility_marker },
      { "typedef", &Parser::parse_typedef },
      { "enum", &Parser::parse_enum_definition },
      { "struct", &Parser::parse_struct },
      { "union", &Parser::parse_union },
    };
};

}
