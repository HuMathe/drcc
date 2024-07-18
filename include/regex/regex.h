#ifndef DRREGEX_H
#define DRREGEX_H

#include "regex/fsm.h"

#include <set>

namespace Regex 
{

/// @brief the Regular Expression Pattern Object, use underlying FSMs to indicate the pattern 
class RegexPatObj
{
public:

    /// @brief regex from a nfa
    /// @param nfa : the Non-determined Finite Automata
    RegexPatObj(const FSM::NFA & nfa);

    /// @brief regex from a fixed patterns (without parameters and operations)
    /// @param pattern : the finite fixed pattern 
    RegexPatObj(const char * const pattern);

    /// @brief regex pattern from a set
    /// @param acc_char_set : accepted characters
    RegexPatObj(const std::set<char> & acc_char_set);


    /// @brief regex pattern matching a range of characters [first, last],
    ///          swap if (first > last)
    /// @param range_first : the first characther in the range 
    /// @param range_last : the last character in the range
    RegexPatObj(char range_first, char range_last);

    /// @brief find the longest prefix of a string that matches the regex
    /// @param str : the target string
    /// @param length : the length of the target string
    /// @return the length of the larges prefix of `str` that matches
    int max_matched_lenghth(const char * const str, int length) const;

    /// @brief generate C = AB
    /// @param rhs : the right-hand-side reg
    /// @return the concatenated regex
    RegexPatObj operator+ (const RegexPatObj & rhs) const;

    /// @brief generate C = A | B
    /// @param rhs : the right-hand-side reg
    /// @return the unioned regex
    RegexPatObj operator| (const RegexPatObj & rhs) const;

    /// @brief generate C = A{m}
    /// @param times : the times of repetition
    /// @return the repeated regex
    RegexPatObj operator* (int times) const;

    /// @brief assigning operator
    /// @param src 
    /// @return *this to support chaining operation
    RegexPatObj & operator= (const RegexPatObj & src);

    /// @brief generate C = A{m}A* 'friend'
    RegexPatObj friend RegexIter(const RegexPatObj & regex, int min_times);

    /// @brief compile the nfa into dfa
    void compile();

private:

    /// @brief true if dfa is built
    bool compiled_;

    /// @brief the nfa (used to build the regex)
    FSM::NFA nfa_;

    /// @brief dfa : used to optimize and match targets
    FSM::DFA dfa_;
};

/// @brief generate C = A{m}A*
/// @param regex : the base regex
/// @param min_times : the minimum repeate time, 0 if A*
/// @return the iteration regex
RegexPatObj RegexIter(const RegexPatObj & regex, int min_times = 0);


}




#endif