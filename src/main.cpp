#include "scanner.h"
#include "parser.h"
#include "code_gen.h"
#include "miscs.h"

#include <iostream>
#include <fstream>

// Durable and Rubust C Compiler (Derong C Compiler)
using namespace DRCC;

int main(int argc, char **argv)
{
    // the parser take the owership of its scanner
    Parser parser(std::make_unique<Scanner>(std::cin));

    // parsing
    auto root = parser.parse();
    // export the parse tree in the form of graphviz file
    if(argc == 2)
    {
        std::ofstream fout(argv[1]);
        export_graphviz(root, fout);
        fout.close();
    }

    // code generation to stdout
    mipsCodeGen code(root);
    code.generate(std::cout);

    return 0;
} 