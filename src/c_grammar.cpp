/**
 * @file c_grammar.cpp
 * @brief This file defines the LR(1) context free grammar for simplified C language 
 */

#include "parser.h"

namespace DRCC
{
/// @brief Helper funtion, generate the C grammar
/// @return c grammar list
Grammar c_grammar()
{
    Grammar g = {
        {goal, {program}},
        {program, {var_declarations, statements}},
        {var_declarations, {var_declarations, var_declaration}},
        {var_declarations, {}},
        {var_declaration, {INT, declaration_list, SEMI}},
        {declaration_list, {declaration_list, COMMA, declaration}},
        {declaration_list, {declaration}},
        {declaration, {ID, ASSIGN, INT_NUM}},
        {declaration, {ID, LSQUARE, INT_NUM, RSQUARE}},
        {declaration, {ID}},
        {code_block, {LBRACE, statements, RBRACE}},
        {statements, {statement}},
        {statements, {statements, statement}},
        {statement, {open_stmt}},
        {statement, {closed_stmt}},
        {closed_stmt, {simple_stmt}},
        {closed_stmt, {IF, LPAR, exp, RPAR, closed_stmt, ELSE, closed_stmt}},
        {closed_stmt, {WHILE, LPAR, exp, RPAR, closed_stmt}},
        {open_stmt, {IF, LPAR, exp, RPAR, statement}},
        {open_stmt, {IF, LPAR, exp, RPAR, closed_stmt, ELSE, open_stmt}},
        {open_stmt, {WHILE, LPAR, exp, RPAR, open_stmt}},
        {simple_stmt, {assign_stmt, SEMI}},
        {simple_stmt, {ctrl_stmt}},
        {simple_stmt, {io_stmt, SEMI}},
        {simple_stmt, {code_block}},
        {simple_stmt, {exp, SEMI}},
        {simple_stmt, {SEMI}},
        {ctrl_stmt, {do_while_stmt, SEMI}},
        {ctrl_stmt, {return_stmt, SEMI}},
        {io_stmt, {read_stmt}},
        {io_stmt, {write_stmt}},
        {assign_stmt, {ID, LSQUARE, exp, RSQUARE, ASSIGN, exp}},
        {assign_stmt, {ID, ASSIGN, exp}},
        {do_while_stmt, {DO, statement, WHILE, LPAR, exp, RPAR}},
        {return_stmt, {RETURN}},
        {read_stmt, {READ, LPAR, ID, RPAR}},
        {write_stmt, {WRITE, LPAR, exp, RPAR}},

        // precedence definations
        {exp, {exp12}},
        {exp12, {exp12, op12, exp11}},
        {exp12, {exp11}},
        {op12, {OROR}},
        {exp11, {exp11, op11, exp10}},
        {exp11, {exp10}},
        {op11, {ANDAND}},
        {exp10, {exp10, op10, exp8}},
        {exp10, {exp8}},
        {op10, {OR_OP}},
        {exp8, {exp8, op8, exp7}},
        {exp8, {exp7}},
        {op8, {AND_OP}},
        {exp7, {exp7, op7, exp6}},
        {exp7, {exp6}},
        {op7, {EQ}},
        {op7, {NOTEQ}},
        {exp6, {exp6, op6, exp5}},
        {exp6, {exp5}},
        {op6, {GT}},
        {op6, {LT}},
        {op6, {GTEQ}},
        {op6, {LTEQ}},
        {exp5, {exp5, op5, exp4}},
        {exp5, {exp4}},
        {op5, {SHL_OP}},
        {op5, {SHR_OP}},
        {exp4, {exp4, op4, exp3}},
        {exp4, {exp3}},
        {op4, {PLUS}},
        {op4, {MINUS}},
        {exp3, {exp3, op3, exp2}},
        {exp3, {exp2}},
        {op3, {MUL_OP}},
        {op3, {DIV_OP}},
        {exp2, {op2, exp2}},
        {exp2, {exp1}},
        {op2, {PLUS}},
        {op2, {MINUS}},
        {op2, {NOT_OP}},
        {exp1, {INT_NUM}},
        {exp1, {ID}},
        {exp1, {ID, LSQUARE, exp, RSQUARE}},
        {exp1, {LPAR, exp, RPAR}},

// extra features        
        {ctrl_stmt, {BREAK, SEMI}},
        {op3, {MOD_OP}},
    };
    return g;
}   

}
