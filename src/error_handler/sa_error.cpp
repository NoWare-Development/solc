#include "error_handler.hpp"
#include "util/util.hpp"
#include <sa/sa.hpp>
#include <types.hpp>

using SAErrorType = nlc::SemanticAnalyzer::SAErrorType;

std::string
ErrorHandler::get_sa_error_reason (
    const nlc::SemanticAnalyzer::SAError &err) const
{
  std::string out{};
  switch (err.type)
    {
    case SAErrorType::NONE:
      out += "nlc::SemanticAnalyzer::SAErrorType::NONE -- This message should "
             "never occur";
      break;

    case SAErrorType::UNDEFINED_TYPE:
      out += "unknown type \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += _tokens.at (err.token_positions.at (0)).value;
      out += escape_reset ();
      out += "\"";
      break;

    case SAErrorType::TYPE_REDEF:
      out += "redefinition of type \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += _tokens.at (err.token_positions.at (0)).value;
      out += escape_reset ();
      out += "\"";
      break;

    case SAErrorType::VAR_REDECL:
      out += "redeclaration of variable \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += _tokens.at (err.token_positions.at (0)).value;
      out += escape_reset ();
      out += "\"";
      break;

    case SAErrorType::SYMBOL_REDEF:
      out += "redefinition of symbol \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += _tokens.at (err.token_positions.at (0)).value;
      out += escape_reset ();
      out += "\"";
      break;

    default:
      out = "<Unknown semantic analyzer error>";
      break;
    }

  return out;
}
