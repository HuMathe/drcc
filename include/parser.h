#ifndef DRCC_PARSER_H
#define DRCC_PARSER_H

#include "tokenType.h"
#include "scanner.h"
#include "symbols.h"

#include <vector>
#include <map>
#include <set>
#include <memory>
#include <unordered_map>

namespace DRCC
{

// production rule type
struct Production
{
    Symbol lhs;
    std::vector<Symbol> rhs;
};

enum class ActionEntryEnum : unsigned char
{
    SHIFT,
    REDUCE,
    ACCEPT,
    ERROR,
};

/// @brief action table entry
struct ActionEntry
{
    ActionEntryEnum action;
    int target;
    bool operator== (const ActionEntry & rhs) const;
    bool operator!= (const ActionEntry & rhs) const;
};


/// @brief Abstract Syntax Tree Node
class ASTNode
{
public:

    /// @brief the terminal or non-terminal symbol represented by the node
    Symbol symbol;

    /// @brief the production rule index that the node is reduced from
    /// @attention applicable only if the current symbol is a non-terminal symbol
    int prod_idx;

    /// @brief the lexem the current token is carrying
    /// @attention applicable only if the current symbo is a teminal symbol (token)
    std::string lexeme;

    /// @brief the child nodes that fits the right-hand-side of the current production rule
    /// @attention applicable only if the current symbol is a non-terminal symbol
    std::vector<std::shared_ptr<ASTNode>> children;

public:
    ASTNode(Token tok);
    ASTNode(NonTerminal symb, int prod_idx, 
        const std::vector<std::shared_ptr<ASTNode>> children);
};

// LR(1) item
class LR1Item
{
public:
    int prod_idx;
    int position;
    Terminal lookahead;

    LR1Item(int prod_idx, int position, Terminal lookahead);
    bool operator<(const LR1Item & rhs) const;
};

typedef std::vector<Production> Grammar;
typedef std::set<LR1Item> LR1ItemSet;
typedef std::map<LR1ItemSet, int> LR1ItemSetId;
typedef std::map<int, std::map<Symbol, int>> GotoTable;

class Parser
{
private:
    /// @brief initial state in the parsing process
    int initial_state;

    /// @brief the scanner
    std::unique_ptr<Scanner> scanner;

    /// @brief The set of grammar (C-grammar in this project)
    Grammar grammar;


    std::map<Symbol, std::vector<int>> _gmap;
    std::map<Symbol, std::set<Terminal>> _first_set;
    std::set<Symbol> symbols;
    std::set<Symbol> _terminals;
    std::set<Symbol> _nonterminals;

    /// @brief map from idx to I_{idx}
    std::vector<LR1ItemSet> _lr1_items;
    
    /// @brief map I to its index
    LR1ItemSetId _lr1_item_idx;

    
    /// @brief memorize goto_function
    GotoTable _goto_archive;

    /// @brief ACTION table in parsing
    std::unordered_map<int, std::map<Terminal, ActionEntry>> __action_transition_tab;

    /// @brief GOTO table in parsing
    std::unordered_map<int, std::map<NonTerminal, int>> _goto_transition_tab;



    /// @brief GOTO table function
    /// @param itemset_idx 
    /// @param X 
    /// @return GOTO(Ii, X)
    int _goto(int itemset_idx, Symbol X);

    /// @brief Compute (in-place) the LR1 closure of the grammar itemset
    /// @param I initial itemset
    LR1ItemSet make_closure(LR1ItemSet & I);

    /// @brief Construct the collection of sets of LR(1) items for the augmented grammar
    void _construct_cannonical_lr1_items();

    /// @brief Compute the first sets of every symbol
    void _compute_first_set();

    /// @brief Construction of canonical-LR parsing tables
    void _construct_lr1_parsing_table();


public:
    Parser(std::unique_ptr<Scanner> scanner);
    std::shared_ptr<ASTNode> parse();
    void init();

    void print_table();
};

/// @brief Helper funtion, generate the C grammar
/// @return c grammar list
Grammar c_grammar();


}


#endif