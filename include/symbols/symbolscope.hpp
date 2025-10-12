#pragma once

#include <memory>
#include <string>
#include <symbols/symbol.hpp>
#include <unordered_map>

namespace nlc
{

struct SymbolScope
{

  SymbolScope () = default;
  SymbolScope (SymbolScope *parent, Symbol *bound_symbol = nullptr)
      : _bound_symbol (bound_symbol), _parent (parent)
  {
  }

  void
  add_symbol (const std::string &name, std::shared_ptr<Symbol> symbol)
  {
    if (has_symbol (name))
      return;

    symbol->_symbol_name = name;
    symbol->_symbol_scope = this;
    _symbols[name] = symbol;
  }

  bool
  has_symbol (const std::string &name) const
  {
    return _symbols.find (name) != _symbols.end ();
  }

  std::shared_ptr<Symbol>
  get_symbol (const std::string &name)
  {
    return _symbols.at (name);
  }

  Symbol *
  get_bound_symbol () const
  {
    return _bound_symbol;
  }

  SymbolScope *
  get_parent () const
  {
    return _parent;
  }

private:
  std::unordered_map<std::string, std::shared_ptr<Symbol>> _symbols{};
  Symbol *_bound_symbol{};
  SymbolScope *_parent;
};

}
