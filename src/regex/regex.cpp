#include "regex/regex.h"

#include <unordered_map>
#include <cstring>

namespace Regex
{

// initialize by a nfa
RegexPatObj::RegexPatObj(const FSM::NFA & nfa)
    : nfa_(nfa), dfa_(nfa_), compiled_(false)
{

}

// initialize by a fixed pattern
RegexPatObj::RegexPatObj(const char *const pattern)
    : nfa_(std::strlen(pattern) + 1, 0), dfa_(nfa_), compiled_(false)
{
    int length = std::strlen(pattern);
    for(int i = 0; i < length; i++)
    {
        this->nfa_.add_transition(i, pattern[i], i + 1);
    }
    nfa_.add_accept_state(length);
}


// initialize by a set of accepting characters
RegexPatObj::RegexPatObj(const std::set<char> &acc_char_set)
    : nfa_(2, 0), dfa_(nfa_), compiled_(false)
{
    for(char acc_char : acc_char_set)
    {
        nfa_.add_transition(0, acc_char, 1);
    }
    nfa_.add_accept_state(1);
}

// initialize by a contiguous range of accepting characters, e.g., a-z
RegexPatObj::RegexPatObj(char range_first, char range_last)
    : nfa_(2, 0), dfa_(nfa_), compiled_(false)
{
    if(range_last < range_first)
    {
        std::swap(range_last, range_first);
    }

    for(char ch = range_first; ch <= range_last; ch++)
    {
        nfa_.add_transition(0, ch, 1);   
    }
    nfa_.add_accept_state(1);
}

// the longest accept prefix (call the API of dfa directly)
int RegexPatObj::max_matched_lenghth(const char *const target_str, int length) const
{
    return this->dfa_.max_accept_length(target_str, length);
}

// the two regular expressions are apearing in sequential order
RegexPatObj RegexPatObj::operator+(const RegexPatObj &rhs) const
{
    FSM::NFA ret_nfa(nfa_.size() + rhs.nfa_.size(), nfa_.start_state());

    // the left regex
    for(int state = 0; state < nfa_.size(); state++)
    {
        const auto & transition_tab = nfa_.transitions(state);
        for(const auto &kv : transition_tab)
        {
            ret_nfa.add_transitions(state, kv.first, kv.second);
        }
    }


    // place the right regex after the left
    int left_size = nfa_.size();

    for(int state_1 = 0; state_1 < rhs.nfa_.size(); state_1++)
    {
        const auto & transition_tab = rhs.nfa_.transitions(state_1);
        for(const auto &kv : transition_tab)
        {
            for(const int &to : kv.second)
            {
                ret_nfa.add_transition(state_1 + left_size, kv.first, to + left_size);
            }
        }
    }


    // link the two components
    for(int from : nfa_.accept_states())
    {
        ret_nfa.add_transition(from, '\0', rhs.nfa_.start_state() + left_size);
    }

    for(int acc : rhs.nfa_.accept_states())
    {
        ret_nfa.add_accept_state(acc + left_size);
    }
    
    return RegexPatObj(ret_nfa);
}


// both regexs are acceptable
RegexPatObj RegexPatObj::operator|(const RegexPatObj &rhs) const
{
    FSM::NFA ret_nfa(nfa_.size() + rhs.nfa_.size() + 2, nfa_.size() + rhs.nfa_.size());
    int start_state = nfa_.size() + rhs.nfa_.size();
    int final_state = nfa_.size() + rhs.nfa_.size() + 1;

    // left regex
    for(int state = 0; state < nfa_.size(); state++)
    {
        const auto & transition_tab = nfa_.transitions(state);
        for(const auto &kv : transition_tab)
        {
            ret_nfa.add_transitions(state, kv.first, kv.second);
        }
    }

    // right regex
    int left_size = nfa_.size();

    for(int state_1 = 0; state_1 < rhs.nfa_.size(); state_1++)
    {
        const auto & transition_tab = rhs.nfa_.transitions(state_1);
        for(const auto &kv : transition_tab)
        {
            for(const int &to : kv.second)
            {
                ret_nfa.add_transition(state_1 + left_size, kv.first, to + left_size);
            }
        }
    }


    // link to the start state and accept states
    ret_nfa.add_transition(start_state, '\0', nfa_.start_state());
    for(int acc : nfa_.accept_states())
    {
        ret_nfa.add_transition(acc, '\0', final_state);
    }

    // link to the start state and accept states
    ret_nfa.add_transition(start_state, '\0', rhs.nfa_.start_state() + left_size);
    for(int acc : rhs.nfa_.accept_states())
    {
        ret_nfa.add_transition(acc + left_size, '\0', final_state);
    }

    // set accept state
    ret_nfa.add_accept_state(final_state);

    return RegexPatObj(ret_nfa);
}

// 0th iterations
RegexPatObj RegexPatObj::operator*(int times) const
{
    RegexPatObj ans(""), a = *this;

    ///@todo 1. optimize the multiplication by using O(log N) times of additions
    ///@todo 2. optimize the addition by using inplace-addition
    for(int i = 0; i < times; i++)
    {
        ans = ans + a;
    }

    return ans;
}


// deep copy of a regex
RegexPatObj &RegexPatObj::operator=(const RegexPatObj &src)
{
    if(this == &src)
    {
        return *this;
    }

    this->nfa_ = src.nfa_;
    this->nfa_.degrade();
    this->dfa_ = FSM::DFA(this->nfa_);
    this->compiled_ = false;

    return *this;
}

// compile the nfa to dfa
void RegexPatObj::compile()
{
    
    if(this->compiled_)
    {
        return ;
    }

    nfa_.build();
    dfa_ = FSM::DFA(nfa_);
    dfa_.build();

    this->compiled_ = true;

    return ;
}

// iteration operation
RegexPatObj RegexIter(const RegexPatObj &regex, int min_times)
{
    RegexPatObj loop_regex = regex;
    
    // make regex*
    for(int state : regex.nfa_.accept_states())
    {
        loop_regex.nfa_.add_transition(state, '\0', loop_regex.nfa_.start_state());
    }
    loop_regex.nfa_.add_accept_state(loop_regex.nfa_.start_state());

    // regex{min_times}regex*
    return (regex * min_times) + loop_regex;
}

}
