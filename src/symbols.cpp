#include "symbols.h"

namespace DRCC
{
Symbol::Symbol(Terminal t)
    : is_terminal_(true)
{
    this->type_.t = t;
}

Symbol::Symbol(NonTerminal n)
    : is_terminal_(false)
{
    this->type_.n = n;
}

Symbol::Symbol(std::pair<bool, int> symb_key)
    : is_terminal_(symb_key.first)
{
    if(symb_key.first)
    {
        this->type_.t = static_cast<Terminal>(symb_key.second);
    }
    else
    {
        this->type_.n = static_cast<NonTerminal>(symb_key.second);
    }
}

bool Symbol::operator==(const Symbol &rhs) const
{
    return as_key() == rhs.as_key();
}

bool Symbol::operator==(const Terminal &rhs) const
{
    return this->is_terminal_ && (this->type_.t == rhs);
}

bool Symbol::operator==(const NonTerminal &rhs) const
{
    return !this->is_terminal_ && this->type_.n == rhs;
}

bool Symbol::operator<(const Symbol &rhs) const
{
    return as_key() < rhs.as_key();
}

bool Symbol::is_terminal() const
{
    return is_terminal_;
}

int Symbol::symbol_value() const
{
    if(is_terminal_)
    {
        return this->type_.t;
    }
    return this->type_.n;
}

std::pair<bool, int> Symbol::as_key() const
{
    return std::make_pair(is_terminal(), symbol_value());
}

Terminal Symbol::as_terminal() const
{
    return type_.t;
}

NonTerminal Symbol::as_nonterminal() const
{
    return type_.n;
}

}
