#include "symbols/symboltable.hpp"
#include <stdexcept>

namespace nlc
{

bool
SymbolTable::has_entry (const std::vector<std::string> &path,
                        const std::string &name) const
{
  return get_entry (path, name) != nullptr;
}

void
SymbolTable::add_entry (const std::vector<std::string> &path,
                        const std::string &name, std::unique_ptr<Symbol> entry)
{
  if (has_entry (path, name))
    {
      std::string str_path{};
      for (const auto &p : path)
        {
          str_path += p + "::";
        }
      str_path += name;

      throw std::invalid_argument ("SymbolData \"" + str_path
                                   + "\" already exists");
    }
}

const SymbolTable::Symbol *
SymbolTable::get_entry (const std::vector<std::string> &path,
                        const std::string &name) const
{
  if (path.empty ())
    {
      return _symbols.find (name) != _symbols.end ()
                 ? _symbols.at (name).get ()
                 : nullptr;
    }

  Symbol *current = nullptr;
  for (const auto &p : path)
    {
      if (current == nullptr)
        {
          if (_symbols.find (p) == _symbols.end ())
            return nullptr;

          current = _symbols.at (p).get ();
        }
      else
        {
          if (!current->has_child (p))
            return nullptr;

          current = const_cast<Symbol *> (current->get_child (p));
        }
    }

  return current->get_child (name);
}

}
