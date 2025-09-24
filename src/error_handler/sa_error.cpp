#include "error_handler.hpp"
#include "util/util.hpp"
#include <sa/sa.hpp>

std::string
ErrorHandler::get_sa_error_reason (
    const nlc::SemanticAnalyzer::SAError &err) const
{
  std::string out{};
  switch (err.type)
    {
    case nlc::SemanticAnalyzer::SAErrorType::UNDEFINED_TYPE:
      out += "Unknown type \"";
      out += escape_graphics (ESCGraphics::ESCGRAPHICS_BOLD);
      out += _tokens.at (err.tok_pos).value;
      out += escape_reset ();
      out += "\"";
      break;

    default:
      out = "<Unknown semantic analyzer error>";
      break;
    }

  return out;
}
