#ifndef DRCC_CODEGEN_H
#define DRCC_CODEGEN_H

#include "parser.h"

#include <iostream>

namespace DRCC
{

/// @brief Code Generation
class mipsCodeGen
{
private:
    /// @brief symbol table: name -> offset
    std::map<std::string, int> symbol_table;

    /// @brief the pointer to the root node of Abstract Syntax Tree (AST)
    std::shared_ptr<ASTNode> root;
    
    /// @brief the total offset of all variables
    int tot_offset;

    /// @brief the counter of labels (for branch/jump instruction)
    int label_cnt;

    /// @brief compile-time code generation process
    /// @param node_ptr the root of the current AST subtree
    /// @param nt the number of temperary variables used currently
    /// @param os the output stream where to push the MIPS assembly code into
    void cgen_(std::shared_ptr<ASTNode> node_ptr, int nt, std::ostream &os);

    /// @brief pre-calculating symbol table
    /// @param node_ptr the root of current AST
    void build_symbol_table_(std::shared_ptr<ASTNode> node_ptr);
public:

    /// @brief Initialization of the generator (with the AST of the program) 
    /// @param node_ptr the root node of the program's AST
    mipsCodeGen(std::shared_ptr<ASTNode> node_ptr);

    /// @brief Generate the MIPS code and push the asm code into stream `os` 
    /// @param os the stream where to push resulting code.
    void generate(std::ostream &os);
};

    
}


#endif