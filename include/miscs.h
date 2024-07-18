#ifndef DRCC_MISCS_H
#define DRCC_MISCS_H

#include <map>
#include <string>

#include <ostream>

#include "symbols.h"
#include "parser.h"
#include "scanner.h"

namespace DRCC
{

/// @brief covert terminal and nonterminal symbols to string (for visualization purpose)
/// @param symb: Symbol Class object 
/// @return its name in std::string
std::string to_string(Symbol symb);


/// @brief export the parse tree structure in graphviz format, the function 
///         does not examin if the graph is in tree structure
/// @param node_ptr tree root
/// @param os 
void export_graphviz(std::shared_ptr<ASTNode> node_ptr, std::ostream &os);


/// @brief print the tokens in the content to `stdout`
/// @param str the code
void show_all_tokens(const std::string & str);

}

#endif