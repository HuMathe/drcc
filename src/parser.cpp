#include <stack>
#include <queue>
#include <memory>
#include <algorithm>

#define DEBUG
#ifdef DEBUG
#include <iostream>
#endif

#include "miscs.h"
#include "parser.h"

namespace DRCC
{

struct stack_state_t
{
    int state;
    std::shared_ptr<ASTNode> ast_node;
};

Parser::Parser(std::unique_ptr<Scanner> scanner)
    : scanner(std::move(scanner))
{
    init();
}


std::shared_ptr<ASTNode> Parser::parse()
{
    Token a = scanner->next_token();
    stack_state_t s {.state = initial_state}, t;

    std::vector<stack_state_t> parse_stack;
    parse_stack.push_back(s);
    
    while(true)
    {
        s = parse_stack.back();
        

        // error state
        if(__action_transition_tab[s.state].count(a.token_type) == 0)
        {
            std::cerr << "syntax error" << std::endl;
            return nullptr;
        }

        const auto & entry = __action_transition_tab[s.state][a.token_type];

        int pop_amt;
        NonTerminal A;
        std::vector<std::shared_ptr<ASTNode>> children;
        switch (entry.action)
        {
        case ActionEntryEnum::SHIFT:
            t.state = entry.target;
            t.ast_node = std::make_shared<ASTNode>(a);
            a = scanner->next_token();
            parse_stack.push_back(t);

            break;

        case ActionEntryEnum::REDUCE:
            pop_amt = grammar[entry.target].rhs.size();
            A = grammar[entry.target].lhs.as_nonterminal();
            
            while(pop_amt --> 0) 
            { 
                children.push_back(parse_stack.back().ast_node);
                parse_stack.pop_back(); 
            } 

            std::reverse(children.begin(), children.end());
            t = parse_stack.back();
            t.state = _goto_transition_tab[t.state][A];
            
            // if(children.size() != 1)
            t.ast_node = std::make_shared<ASTNode>(A, entry.target, children);
            // else
            //     t.ast_node = children[0];

            parse_stack.push_back(t);

            break;
        
        case ActionEntryEnum::ACCEPT:
            
            return s.ast_node;
        
        default:
            // Rejected
            std::cerr << "syntax error" << std::endl;
            return nullptr;
        }
    }

    return parse_stack.back().ast_node;
}

// initialization of the parser
void Parser::init()
{
    this->grammar = c_grammar();
    
    // construct gmap
    for(int i = 0; i < this->grammar.size(); i++)
    {
        _gmap[grammar[i].lhs].push_back(i);
    }

    for(auto & i : this->grammar)
    {
        symbols.insert(i.lhs);
        for(auto & j: i.rhs)
        {
            symbols.insert(j);
        }
    }

    for(auto symb : symbols)
    {
        if(symb.is_terminal())
        {
            _terminals.insert(symb);
        }
        else
        {
            _nonterminals.insert(symb);
        }
    }
    _terminals.emplace(END);

    _compute_first_set();
    _construct_cannonical_lr1_items();
    _construct_lr1_parsing_table();
    
}


// print action & goto table (for debug purpose)
void Parser::print_table()
{
    // no code
}

LR1Item::LR1Item(int prod_idx, int position, Terminal lookahead)
    : prod_idx(prod_idx), position(position), lookahead(lookahead)
{

}

bool LR1Item::operator<(const LR1Item &rhs) const
{
    if(this->lookahead != rhs.lookahead)
    {
        return this->lookahead < rhs.lookahead;
    }
    
    if(this->position != rhs.position)
    {
        return this->position < rhs.position;
    }

    return this->prod_idx < rhs.prod_idx;
}

int Parser::_goto(int itemset_idx, Symbol X)
{
    if(_goto_archive.count(itemset_idx) && _goto_archive[itemset_idx].count(X))
    {
        return _goto_archive[itemset_idx][X];
    }


    LR1ItemSet J;
    for(auto item: _lr1_items[itemset_idx])
    {
        if(item.position < grammar[item.prod_idx].rhs.size() \
            && grammar[item.prod_idx].rhs[item.position] == X)
        {
            item.position += 1;
            J.insert(item);
        }
    }

    make_closure(J);

    if(J.size() == 0)
    {
        return (_goto_archive[itemset_idx][X] = -1);
    }

    if(_lr1_item_idx.count(J) == 0)
    {
        _lr1_item_idx[J] = _lr1_items.size();
        _lr1_items.push_back(J);
    }

    return (_goto_archive[itemset_idx][X] = _lr1_item_idx[J]);
}

LR1ItemSet Parser::make_closure(LR1ItemSet &I)
{
    std::queue<LR1Item> new_items;
    for(const auto & i : I)
    {
        new_items.push(i);
    }

    while(!new_items.empty())
    {
        LR1Item cur = new_items.front();
        new_items.pop();
        
        Production & p_cur = grammar[cur.prod_idx];

        if(cur.position >= p_cur.rhs.size())
        {
            continue;
        }

        Symbol B = p_cur.rhs[cur.position];
        auto candidates_iter = _gmap.find(B);

        if(candidates_iter == _gmap.end())
        {
            continue;
        }

        for(int prod_idx : candidates_iter->second)
        {
            bool nullable = true;
            for(int i = cur.position + 1; i < p_cur.rhs.size(); i++)
            {
                Symbol now = p_cur.rhs[i];

                if(_first_set.count(now) == 0)
                {
                    // not expect to happen:
                    std::cerr << "help, the first set is incomplete." << std::endl;
                    continue;
                }               

                for(auto b : _first_set[now])
                {
                    if(b == NOTOK)
                    {
                        continue;
                    }

                    LR1Item new_item(prod_idx, 0, b);
                    if(I.count(new_item) == 0)
                    {
                        I.insert(new_item);
                        new_items.push(new_item);
                    }

                }

                if(_first_set[now].count(NOTOK) == 0)
                {
                    nullable = false;
                    break;
                }
            }

            if(nullable)
            {
                LR1Item new_item(prod_idx, 0, cur.lookahead);
                if(I.count(new_item) == 0)
                {
                    I.insert(new_item);
                    new_items.push(new_item);
                }
            }
        }
    }

    return I;
}

void Parser::_construct_cannonical_lr1_items()
{
    LR1ItemSet s = {
        {0, 0, END}
    };
    make_closure(s);

    _lr1_item_idx[s] = _lr1_items.size();
    _lr1_items.push_back(s);

    for(int i = 0; i < _lr1_items.size(); i++)
    {
        for(Symbol symb : symbols)
        {
            _goto(i, symb);
        }
    }
       
}

void Parser::_compute_first_set()
{
    for(Symbol symb : _terminals)
    {
        _first_set[symb].insert(static_cast<Terminal>(symb.symbol_value()));
    }

    for(Symbol symb : _nonterminals)
    {
        if(_gmap.count(symb) == 0)
        {
            std::cerr << "Help!" << std::endl;
            continue;
        }

        for(int prod_idx : _gmap[symb])
        {
            if(grammar[prod_idx].rhs.size() == 0)
            {
                _first_set[symb].insert(NOTOK);
            }
        }
    }

    bool changed = true; // loop until no changes.
    while(changed)
    {
        changed = false;
        for(const auto & g : grammar)
        {
            Symbol symb = g.lhs;
            int origin_size = _first_set[symb].size();
            bool nullable = true;

            for(Symbol rsymb : g.rhs)
            {
                _first_set[symb].insert(_first_set[rsymb].begin(), _first_set[rsymb].end());

                if(_first_set[rsymb].count(NOTOK) == 0)
                {
                    nullable = false;
                    break;
                }
            }

            if(nullable)
            {
                _first_set[symb].insert(NOTOK);
            }

            changed = changed || (_first_set[symb].size() != origin_size);
        }
    }

}

void Parser::_construct_lr1_parsing_table()
{
    for(int i = 0; i < _lr1_items.size(); i++)
    {
        auto &action_row = __action_transition_tab[i];
        auto & goto_row = _goto_transition_tab[i];
        auto GOTO_entry = _goto_archive[i];

        // compute shift action
        for(const auto & kv : GOTO_entry)
        {
            if(kv.second == -1)
            {
                continue;
            }

            if(kv.first.is_terminal())
            {
                ActionEntry ae = {
                    .action = ActionEntryEnum::SHIFT, 
                    .target = kv.second,
                };
                Terminal t = kv.first.as_terminal();
                
                action_row[t] = ae;
            }
            else
            {
                goto_row[kv.first.as_nonterminal()] = kv.second;
            }
        }

        // compute reduce action
        for(const auto & items : _lr1_items[i])
        {
            auto &g = grammar[items.prod_idx];
            if(g.lhs == goal)
            {
                if(items.lookahead == END && items.position == 0)
                {
                    initial_state = i;
                }
                if(items.lookahead == END && items.position == 1)
                {
                    action_row[END] = { .action = ActionEntryEnum::ACCEPT };
                }
            }

            if(items.position < g.rhs.size())
            {
                continue;
            }

            // report conflicts
            if(action_row.count(items.lookahead) != 0)
            {
                if(action_row[items.lookahead].action == ActionEntryEnum::SHIFT)
                {
                    std::cerr << "Shift/reduce conflict!" << std::endl;
                }
                else if(action_row[items.lookahead].target != items.prod_idx)
                {
                    std::cerr << "Reduce/reduce conflict!" << std::endl;
                }
                continue;
            }
            
            // set to reduce
            action_row[items.lookahead] = {
                .action = ActionEntryEnum::REDUCE,
                .target = items.prod_idx
            };

        }
    }


}

bool ActionEntry::operator!=(const ActionEntry &rhs) const
{
    return !(*this == rhs);
}

bool ActionEntry::operator==(const ActionEntry &rhs) const
{
    return this->action == rhs.action && this->target == rhs.target;
}

ASTNode::ASTNode(Token tok)
    : symbol(tok.token_type), lexeme(tok.lexeme), prod_idx(-1)
{

}

ASTNode::ASTNode(NonTerminal symb, int prod_idx, const std::vector<std::shared_ptr<ASTNode>> children)
    : symbol(symb), prod_idx(prod_idx), children(children)
{

}


}
