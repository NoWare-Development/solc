#pragma once

#include <memory>
#include <string>
#include <symbols/symbol.hpp>
#include <unordered_map>
#include <vector>

namespace nlc
{

struct SymbolTable
{
  struct Symbol
  {
  public:
    enum class Kind
    {
      NONE = 0,
      FILE,
      FUNCTION,
      STRUCT,
      UNION,
      ENUM,
    };

    Symbol (Symbol *parent, Kind kind, std::unique_ptr<SymbolData> data)
        : _data (std::move (data)), _parent (parent), _kind (kind)
    {
    }
    ~Symbol () = default;

    static std::shared_ptr<Symbol>
    create_node (Symbol *parent, Kind kind,
                 std::unique_ptr<SymbolData> data = {})
    {
      return std::make_shared<Symbol> (parent, kind, std::move (data));
    }

    bool
    is_symbol () const
    {
      return _data != nullptr;
    }

    const std::unique_ptr<SymbolData> &
    get_symbol () const
    {
      return _data;
    }

    bool
    has_child (const std::string &name) const
    {
      return _children.find (name) != _children.end ();
    }

    void
    add_child (const std::string &name, std::unique_ptr<Symbol> node)
    {
      _children[name] = std::move (node);
    }

    const Symbol *
    get_child (const std::string &name) const
    {
      return _children.at (name).get ();
    }

    Kind
    get_kind () const
    {
      return _kind;
    }

    const Symbol *
    get_parent () const
    {
      return _parent;
    }

  private:
    std::unordered_map<std::string, std::unique_ptr<Symbol>> _children{};
    std::unique_ptr<SymbolData> _data;
    Symbol *_parent;

    Kind _kind;
  };

  bool has_entry (const std::vector<std::string> &path,
                  const std::string &name) const;
  void add_entry (const std::vector<std::string> &path,
                  const std::string &name, std::unique_ptr<Symbol> entry);
  const Symbol *get_entry (const std::vector<std::string> &path,
                           const std::string &name) const;

private:
  std::unordered_map<std::string, std::unique_ptr<Symbol>> _symbols{};
};

}
