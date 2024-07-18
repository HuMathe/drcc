#ifndef DRCC_SYMBOLS_H
#define DRCC_SYMBOLS_H

#include "tokenType.h"

namespace DRCC
{

typedef TokenType Terminal;

// non-terminal symbols
enum NonTerminal
{
    program,
    var_declaration,
    var_declarations,
    declaration_list,
    declaration,
    code_block,
    statement,
    statements,
    ctrl_stmt,
    io_stmt,
    assign_stmt,
    do_while_stmt,
    return_stmt,
    read_stmt,
    write_stmt,
    exp,
    simple_stmt,
    open_stmt,
    closed_stmt,
    goal,

    // no exp9, since ^ is not defined
    // these symbols are defined to implement operator precedence
    exp1,
    exp2,   op2, // exp2 -> exp1 | op2 exp2
    exp3,   op3, // exp3 -> exp2 | exp3 op3 exp3
    exp4,   op4, // exp4 -> exp3 | exp4 op4 exp4
    exp5,   op5, // 
    exp6,   op6,
    exp7,   op7,
    exp8,   op8,
    exp10,  op10,
    exp11,  op11,
    exp12,  op12,
};

/// @brief symbol wrapper
union symbol_type_
{
    Terminal t;
    NonTerminal n;
};

/// @brief Terminal & Nonterminal symbol
class Symbol
{
public:
    Symbol(Terminal t);
    Symbol(NonTerminal n);
    Symbol(std::pair<bool, int> symb_key);

    bool operator==(const Symbol &rhs) const;
    bool operator==(const Terminal &rhs) const;
    bool operator==(const NonTerminal &rhs) const;
    bool operator<(const Symbol &rhs) const;
    
    bool is_terminal() const;
    int symbol_value() const;
    std::pair<bool, int> as_key() const;
    
    Terminal as_terminal() const;
    NonTerminal as_nonterminal() const;
private:
    bool is_terminal_;
    symbol_type_ type_;
};


}

#endif