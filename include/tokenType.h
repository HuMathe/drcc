#ifndef DRCC_TOKTYPE_H
#define DRCC_TOKTYPE_H

#include <string>

namespace DRCC
{

/// @brief Simplified C token types
enum TokenType
{
    INT, MAIN, VOID, BREAK, DO, ELSE, IF, WHILE, RETURN, READ, WRITE,

    LBRACE, RBRACE, LSQUARE, RSQUARE, LPAR, RPAR, SEMI, PLUS, MINUS, 
    MUL_OP, DIV_OP, MOD_OP, AND_OP, OR_OP, NOT_OP, ASSIGN, LT, GT, SHL_OP, 
    SHR_OP, EQ, NOTEQ, LTEQ, GTEQ, ANDAND, OROR, COMMA,

    INT_NUM, ID,

    NOTOK, END
};


/// @brief token
struct Token
{
    TokenType token_type;
    std::string lexeme;
};


}

#endif