#include "miscs.h"

#include <iostream>

namespace DRCC
{

std::string to_string(Symbol symb)
{
    if(symb.is_terminal())
    {
        switch (symb.as_terminal())
        {
        case INT: return "INT";
        case MAIN: return "MAIN";
        case VOID: return "VOID";
        case BREAK: return "BREAK";
        case DO: return "DO";
        case ELSE: return "ELSE";
        case IF: return "IF";
        case WHILE: return "WHILE";
        case RETURN: return "RETURN";
        case READ: return "READ";
        case WRITE: return "WRITE";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case LSQUARE: return "LSQUARE";
        case RSQUARE: return "RSQUARE";
        case LPAR: return "LPAR";
        case RPAR: return "RPAR";
        case SEMI: return "SEMI";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MUL_OP: return "MUL_OP";
        case DIV_OP: return "DIV_OP";
        case AND_OP: return "AND_OP";
        case OR_OP: return "OR_OP";
        case NOT_OP: return "NOT_OP";
        case ASSIGN: return "ASSIGN";
        case LT: return "LT";
        case GT: return "GT";
        case SHL_OP: return "SHL_OP";
        case SHR_OP: return "SHR_OP";
        case EQ: return "EQ";
        case NOTEQ: return "NOTEQ";
        case LTEQ: return "LTEQ";
        case GTEQ: return "GTEQ";
        case ANDAND: return "ANDAND";
        case OROR: return "OROR";
        case COMMA: return "COMMA";
        case INT_NUM: return "INT_NUM";
        case ID: return "ID";
        case NOTOK: return "NOTOK";
        case END: return "EOF";
        default:
            break;
        }
    }
    else
    {
        switch (symb.as_nonterminal())
        {
        case program: return "program";
        case var_declaration: return "var_declaration";
        case var_declarations: return "var_declarations";
        case declaration_list: return "declaration_list";
        case declaration: return "declaration";
        case code_block: return "code_block";
        case statement: return "statement";
        case statements: return "statements";
        case ctrl_stmt: return "ctrl_stmt";
        case io_stmt: return "io_stmt";
        case assign_stmt: return "assign_stmt";
        case do_while_stmt: return "do_while_stmt";
        case return_stmt: return "return_stmt";
        case read_stmt: return "read_stmt";
        case write_stmt: return "write_stmt";
        case exp: return "exp";
        case simple_stmt: return "simple_stmt";
        case open_stmt: return "open_stmt";
        case closed_stmt: return "closed_stmt";
        case goal: return "goal";
        case exp1: return "exp1";
        case exp2: return "exp2";
        case op2: return "op2";
        case exp3: return "exp3";
        case op3: return "op3";
        case exp4: return "exp4";
        case op4: return "op4";
        case exp5: return "exp5";
        case op5: return "op5";
        case exp6: return "exp6";
        case op6: return "op6";
        case exp7: return "exp7";
        case op7: return "op7";
        case exp8: return "exp8";
        case op8: return "op8";
        case exp10: return "exp10";
        case op10: return "op10";
        case exp11: return "exp11";
        case op11: return "op11";
        case exp12: return "exp12";
        case op12: return "op12";
        default: 
            break;
        }
    }

    return "UNKNOWN";
}

/// @brief Helper function
/// @param node_ptr 
/// @param os 
void export_graphviz_(std::shared_ptr<ASTNode> node_ptr, std::ostream &os)
{
    os << "\t\t" << (long long) node_ptr.get() \
        << "[label=\"" \
        << (node_ptr->symbol.is_terminal()
            ? to_string(node_ptr->symbol) + '(' + node_ptr->lexeme + ')'
            : to_string(node_ptr->symbol)
        ) \
        << "\"] ;\n";
    
    for(auto child : node_ptr->children)
    {   
        os << "\t\t" << (long long) node_ptr.get() << "--" << (long long) child.get() << ";\n";
        export_graphviz_(child, os);
    }

}   

void export_graphviz(std::shared_ptr<ASTNode> node_ptr, std::ostream &os)
{
    if(node_ptr == nullptr)
    {
        return ;
    }

    os << "graph \"\"\n{\n\tfontname=\"Helvetica,Arial,sans-serif\"\n\tnode [fontname=\"Helvetica,Arial,sans-serif\"]\n\tedge [fontname=\"Helvetica,Arial,sans-serif\"]\n";
    os << "\tsubgraph cluster01\n\t{\n";
    export_graphviz_(node_ptr, os);
    os << "\t}\n}";
}

void show_all_tokens(const std::string &str)
{
    std::cout << "Scanned Tokens:\n";
    Scanner sc(str);
    Token last_tok;
    do
    {
        last_tok = sc.next_token();
        std::cout << to_string(last_tok.token_type) << " ";
    } while (last_tok.token_type != END);
    
    std::cout << '\n' << std::endl; 
}



}


