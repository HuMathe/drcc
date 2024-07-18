#include "regex/fsm.h"

#include <cstdio>
#include <queue>

namespace FSM 
{

DFA::DFA(const NFA &nfa)
    : nfa_p(&nfa), num_states_(0), start_state_(-1),
    accept_states_(), transitions_(), nfa_used_(false), from_nfa_(true)
{
    
}

void DFA::build()
{
    // build the DFA if not yet built
    if(from_nfa_ && nfa_used_ == false)
    {
        build_from_nfa_();
    }
    
}


/// @brief Using a bfs simulates the transition on NFA to generate DFA
void DFA::build_from_nfa_()
{

    // BFS queue
    std::queue<StateSet> state_queue;
    std::map<StateSet, int> dfa_map;

    // the start state of the DFA is the lambda-closure of the start state of NFA
    const StateSet & nfa_start_states = nfa_p->lambda_closure(nfa_p->start_state());
    dfa_map[nfa_start_states] = num_states_++;
    state_queue.push(nfa_start_states);

    while(! state_queue.empty())
    {
        StateSet cur_closure = state_queue.front();
        state_queue.pop();


        // simulate the transition on NFA
        for(char ch = 1; ch > 0; ch++)
        {
            StateSet next_states, next_closure;
            
            // the set of state reached from 1 ch-move
            for(int state : cur_closure)
            {
                const auto & partial = nfa_p->transitions(state).find(ch);
                
                if(partial == nfa_p->transitions(state).end())
                {
                    continue;
                }

                next_states.insert(partial->second.begin(), partial->second.end());
            }

            if(next_states.empty())
            {
                continue;
            }

            // build the set of states reached from 1 ch-move and several lambda-moves
            for(int state : next_states)
            {
                const StateSet & partial = nfa_p->lambda_closure(state);
                next_closure.insert(partial.begin(), partial.end());
            }

            // if the state is not explored, then push into queue
            if(dfa_map.count(next_closure) == 0)
            {
                dfa_map[next_closure] = num_states_++;
                state_queue.push(next_closure);
            }

            transitions_[dfa_map[cur_closure]][ch] = dfa_map[next_closure];

        }
    }


    // the accept state of DFA is all the states containing one accept state of the NFA
    for(const auto & closure : dfa_map)
    {
        for(int state : closure.first)
        {
            if(nfa_p->is_accept_state(state))
            {
                accept_states_.insert(closure.second);
                break;
            }
        }
    }
    
    
    start_state_ = dfa_map.at(nfa_p->lambda_closure(nfa_p->start_state()));

}

int DFA::max_accept_length(const char * const s, int max_length) const
{
    int max_ans = 0;
    int now_state = this->start_state_;
    for(int i = 0; i < max_length; i++)
    {
        char ch = s[i];

        // no corresponding stransition: the matching fails
        if(transitions_.count(now_state) == 0 || transitions_.at(now_state).count(ch) == 0)
        {
            break;
        }

        // transfer through existing rules
        now_state = transitions_.at(now_state).at(ch);
        
        // update the answer if the current state is accepted
        if(accept_states_.count(now_state) != 0)
        {
            max_ans= i + 1;
        }
    }

    return max_ans;
}

// deep copy (see fsm.h)
DFA & DFA::operator= (const DFA & src)
{
    if(this == &src)
    {
        return *this;
    }

    nfa_p = src.nfa_p;
    nfa_used_ = src.nfa_used_;
    from_nfa_ = src.from_nfa_;
    num_states_ = src.num_states_;
    start_state_ = src.start_state_;
    accept_states_ = src.accept_states_;
    transitions_ = src.transitions_;

    return *this;
}

bool DFA::nfa_built() const
{
    return nfa_p->built();
}

}