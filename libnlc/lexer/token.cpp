#include "lexer/token.hpp"

namespace nlc
{

constexpr const char *tokentype_to_string (TokenType type);

std::string
Token::to_string () const
{
  std::string out = "Token { ";

  out += "type: " + std::string (tokentype_to_string (type));

  bool print_value = !value.empty ();
  switch (type)
    {
    case TokenType::ID:
    case TokenType::NUM:
    case TokenType::NUMHEX:
    case TokenType::NUMBIN:
    case TokenType::NUMOCT:
    case TokenType::NUMFLOAT:
    case TokenType::STRING:
    case TokenType::SYMBOL:
      print_value = true;
      break;
    default:
      break;
    }

  if (print_value)
    {
      out += ", value: \"" + value + "\"";
    }

  out += " }";

  return out;
}

constexpr const char *
tokentype_to_string (TokenType type)
{
  switch (type)
    {
    case TokenType::ERR:
      return "ERR";

    case TokenType::ID:
      return "ID";

    case TokenType::NUM:
      return "NUM";
    case TokenType::NUMHEX:
      return "NUMHEX";
    case TokenType::NUMBIN:
      return "NUMBIN";
    case TokenType::NUMOCT:
      return "NUMOCT";
    case TokenType::NUMFLOAT:
      return "NUMFLOAT";

    case TokenType::STRING:
      return "STRING";
    case TokenType::SYMBOL:
      return "SYMBOL";

    case TokenType::LPAREN:
      return "LPAREN";
    case TokenType::RPAREN:
      return "RPAREN";
    case TokenType::LBRACK:
      return "LBRACK";
    case TokenType::RBRACK:
      return "RBRACK";
    case TokenType::LBRACE:
      return "LBRACE";
    case TokenType::RBRACE:
      return "RBRACE";

    case TokenType::LARROW:
      return "LARROW";
    case TokenType::RARROW:
      return "RARROW";

    case TokenType::LTHAN:
      return "LTHAN";
    case TokenType::GTHAN:
      return "GTHAN";

    case TokenType::COLON:
      return "COLON";
    case TokenType::DCOLON:
      return "DCOLON";

    case TokenType::SEMI:
      return "SEMI";

    case TokenType::COMMA:
      return "COMMA";
    case TokenType::PERIOD:
      return "PERIOD";

    case TokenType::BAND:
      return "BAND";
    case TokenType::BOR:
      return "BOR";
    case TokenType::BXOR:
      return "BXOR";
    case TokenType::BNOT:
      return "BNOT";

    case TokenType::AND:
      return "AND";
    case TokenType::OR:
      return "OR";
    case TokenType::NOT:
      return "NOT";
    case TokenType::QUE:
      return "QUE";

    case TokenType::LTHANEQ:
      return "LTHANEQ";
    case TokenType::GTHANEQ:
      return "GTHANEQ";
    case TokenType::NOTEQ:
      return "NOTEQ";

    case TokenType::ADD:
      return "ADD";
    case TokenType::SUB:
      return "SUB";
    case TokenType::MUL:
      return "MUL";
    case TokenType::DIV:
      return "DIV";
    case TokenType::MOD:
      return "MOD";
    case TokenType::SHL:
      return "SHL";
    case TokenType::SHR:
      return "SHR";

    case TokenType::EQ:
      return "EQ";
    case TokenType::ADDEQ:
      return "ADDEQ";
    case TokenType::SUBEQ:
      return "SUBEQ";
    case TokenType::MULEQ:
      return "MULEQ";
    case TokenType::DIVEQ:
      return "DIVEQ";
    case TokenType::MODEQ:
      return "MODEQ";
    case TokenType::BANDEQ:
      return "BANDEQ";
    case TokenType::BOREQ:
      return "BOREQ";
    case TokenType::BXOREQ:
      return "BXOREQ";
    case TokenType::SHLEQ:
      return "SHLEQ";
    case TokenType::SHREQ:
      return "SHREQ";

    case TokenType::EQEQ:
      return "EQEQ";

    case TokenType::AT:
      return "AT";
    case TokenType::HASH:
      return "HASH";

    default:
      return "<Unknown token type>";
    }
}

}
