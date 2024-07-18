#include "regex/fsm.h"

#include <iostream>

namespace FSM
{

NFA::NFA(int num_states, int start_state)
    : num_states_(num_states), lambda_closure_generated_(false),
    transitions_(num_states), lambda_closure_(num_states),
    start_state_(start_state), accept_states_()
{

}

void NFA::add_transition(int from, char ch, int to)
{
    transitions_[from][ch].insert(to);
}

void NFA::add_transitions(int from, char ch, const StateSet & dsts)
{
    // insert a set of destinations
    transitions_[from][ch].insert(dsts.begin(), dsts.end());
}

void NFA::add_accept_state(int state)
{
    accept_states_.insert(state);
}

void NFA::remove_accept_state(int state)
{
    if(accept_states_.count(state) != 0)
    {
        accept_states_.erase(state);
    }
}

const StateSet & NFA::lambda_closure(int state) const
{
    return lambda_closure_[state];
}

const std::unordered_map<char, StateSet> & NFA::transitions(int state) const 
{
    return transitions_[state];
}

const StateSet & NFA::accept_states() const 
{
    return accept_states_;
}

bool NFA::is_accept_state(int state) const
{
    return accept_states_.count(state) != 0;
}

int NFA::start_state() const 
{
    return start_state_;
}

void NFA::set_start_state(int state)
{
    start_state_ = state;
}

int NFA::size() const 
{
    return num_states_;
}

void NFA::build()
{
    // calculate the lambda closure of each state
    for(int i = 0; i < num_states_; i++)
    {
        if(lambda_closure_[i].size() == 0)
        {
            compute_lambda_closure(i);
        }
    }
    lambda_closure_generated_ = true;
}

void NFA::compute_lambda_closure(int state)
{
    // compute the lambda closure in a DFS manner

    StateSet & closure = lambda_closure_[state];
    closure.insert(state);

    // the lambda closure of a state contains the union of lambda-closures of all its lambda-neighbors
    for(int next_state : transitions_[state]['\0'])
    {
        if(closure.count(next_state) != 0)
        {
            continue;
        }

        // insert before search in order to avoid infinte loop
        closure.insert(next_state);
        compute_lambda_closure(next_state);
        closure.insert(lambda_closure_[next_state].begin(), lambda_closure_[next_state].end());
        
        if(is_accept_state(next_state))
        {
            accept_states_.insert(state);
        }

    }
}

bool NFA::built() const
{
    return lambda_closure_generated_;
}

// clear all the computed lambda closures
void NFA::degrade()
{
    if(!lambda_closure_generated_)
    {
        return ;
    }

    lambda_closure_generated_ = false;

    for(StateSet & closure : lambda_closure_)
    {
        closure.clear();
    }
}


// deep copy, see fsm.h
NFA & NFA::operator= (const NFA & src)
{
    if(this == &src)
    {
        return *this;
    }

    start_state_ = src.start_state_;
    num_states_ = src.num_states_;
    lambda_closure_generated_ = src.lambda_closure_generated_;
    accept_states_ = src.accept_states_;
    transitions_ = src.transitions_;
    lambda_closure_ = src.lambda_closure_;

    return *this;
}

}
