#include "libsolc/parser/macros.hpp"
#include "parser/parser.hpp"

namespace solc
{

bool Parser::is_generic_namespace()
{
  if (peek(_pos + 1) != TokenType::LTHAN || _pos + 2 >= _tokens.size())
    return false;

  auto old_pos = _pos;
  _pos += 2;
  bool maybe_generic = false;
  bool errored_prev = _errored;
  auto old_len = _errors.size();

  while (_pos < _tokens.size()) {
    if (peek(_pos) == TokenType::GTHAN) {
      maybe_generic = true;
      _pos++;
      break;
    }

    parse_type();
    if (_errors.size() != old_len) {
      break;
    }

    if (auto next_token = peek(_pos); next_token != TokenType::GTHAN) {
      if (next_token != TokenType::COMMA) {
        break;
      }

      _pos++;
    }
  }

  if (maybe_generic && peek(_pos) == TokenType::DCOLON &&
      peek(_pos + 1) == TokenType::ID) {
    _errors.erase(_errors.begin() + old_len, _errors.end());
    _errored = errored_prev;
    _pos = old_pos;

    return true;
  }

  _errors.erase(_errors.begin() + old_len, _errors.end());
  _errored = errored_prev;
  _pos = old_pos;
  return false;
}

AST Parser::parse_generic_namespace()
{
  VERIFY_POS(_pos);
  auto cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::ID);

  AST out(_pos, ASTType::GENERIC_NAMESPACE, cur.value);

  _pos++;
  VERIFY_POS(_pos);
  cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::LTHAN);

  auto generic_type_list = parse_generic_type_list();

  out.append(generic_type_list);

  VERIFY_POS(_pos);
  cur = _tokens.at(_pos);
  VERIFY_TOKEN(_pos, cur.type, TokenType::DCOLON);
  _pos++;

  return out;
}

}
