#ifndef DR_FINITE_STATE_MACHINE_H
#define DR_FINITE_STATE_MACHINE_H

#include <set>
#include <map>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace FSM
{

typedef std::set<int> StateSet;


/// @brief Non-determined Finite Automation, a NFA contains a set of state and a 
///        general rule that guides the transition between states. In this project,
///        the NFA is used to implement the function of matching a pattern from the given 
///        string (ascii based). A string is accept, if and only if, there exists a 
///        path from the start state of the NFA, ends at one of the accept states, and 
///        the path in between is the characters of the string in correct order.  
class NFA
{
public:

    NFA(int num_states, int start_state = 0);

    /// @brief add transition: (from)--[ch]-->(to) 
    ///         after the operation, state (from) can transfer to (to) by 1 additional (ch) 
    void add_transition(int from, char ch, int to);

    /// @brief states in dsts can be reached from (from) by a new (ch)
    void add_transitions(int from, char ch, const StateSet &dsts);
    
    // add an accept state to the nfa
    void add_accept_state(int state);

    // remove a state from the accept state set of the nfa
    void remove_accept_state(int state);

    /// @brief the set of states that can be reached from the state `state` via '\0'
    /// @param state 
    /// @return the lambda closure
    const StateSet & lambda_closure(int state) const;

    /// @brief the neiboring states of a given state
    /// @param state 
    /// @return the archived transition map
    const std::unordered_map<char, StateSet> & transitions(int state) const;

    /// @brief return `true` if the state is one of the accept states 
    bool is_accept_state(int state) const;

    /// @brief return the archived accept state set 
    const StateSet & accept_states() const;


    /// @brief the start state of the nfa, i.e., where to start matching 
    int start_state() const;


    /// @brief set the start state to a given value
    void set_start_state(int state);

    /// @brief compute the lambda closures of all states
    void build();

    /// @brief return `true` if the lambda closures are built 
    bool built() const;

    /// @brief the number of states in the nfa  
    int size() const;

    /// @brief deep copy assignment of nfa 
    NFA & operator= (const NFA & src);

    /// @brief reduce the nfa and delete all the computed lambda closures
    void degrade();

private:

    /// @brief compute the lambda closure of a given state  
    void compute_lambda_closure(int state);

private:

    /// @brief the state to start matching
    int start_state_;

    /// @brief the number of states in the nfa
    int num_states_;

    /// @brief `true` if the lambda closures are generated
    bool lambda_closure_generated_;

    /// @brief the set of accept states
    StateSet accept_states_;

    /// @brief transition map
    std::vector<std::unordered_map<char, StateSet>> transitions_;

    /// @brief lambda closure of each state
    std::vector<StateSet> lambda_closure_;
};



/// @brief Determined Finite Auotmation
class DFA
{
public:

    /// @brief construct the dfa from a nfa
    /// @param nfa 
    DFA(const NFA & nfa);


    /// @brief match a target string an find out the largest prefix matched
    /// @param str 
    /// @param max_length the length of the buffer
    /// @return the maximum matched length
    int max_accept_length(const char * const str, int max_length) const;


    /// @brief build the dfa from the compiled nfa
    void build();


    /// @brief deep copy assignment 
    DFA & operator= (const DFA & src);

    /// @brief `true` if the nfa is finshed generating `lambda_closure` 
    bool nfa_built() const;

private:

    /// @brief build the dfa from a compiled nfa
    void build_from_nfa_();

private:
    
    /// @brief if the dfa is based on nfa (true in this project)
    bool nfa_used_, from_nfa_;
    
    /// @brief the pointer to the base nfa
    const NFA * nfa_p;

    /// @brief the size of the dfa, determined on `build`
    int num_states_;

    /// @brief the start state of the dfa
    int start_state_;

    /// @brief accept states of the dfa
    StateSet accept_states_;

    /// @brief the transition map
    std::map<int, std::unordered_map<char, int>> transitions_;

    
};



}


#endif