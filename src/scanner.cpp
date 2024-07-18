#include <iostream>

#include <cstring>

#include "scanner.h"
#include "miscs.h"


namespace DRCC
{

void Scanner::add_tokenizer(const char * const pat, TokenType token_type)
{
    Regex::RegexPatObj regex(pat);
    regex.compile();
    this->regexs.emplace_back(
        regex, token_type
    );
}

void Scanner::add_tokenizer(Regex::RegexPatObj regex, TokenType token_type)
{
    regex.compile();
    this->regexs.emplace_back(
        regex, token_type
    );
}

Scanner::Scanner()
{
    content = "";
    init();
}


Scanner::Scanner(std::istream &is)
    : content(std::istreambuf_iterator<char>(is), {})
{
    init();
}

Scanner::Scanner(const std::string & input)
    : content(input)
{
    init();
}


void Scanner::init()
{
    using Regex::RegexPatObj;

    /// regex: [0-9]
    RegexPatObj pat_digits('0', '9');

    /// regex: [a-z]
    RegexPatObj pat_lowercases('a', 'z');

    /// regex: [A-Z]
    RegexPatObj pat_uppercases('A', 'Z');

    /// regex: [a-zA-Z]
    RegexPatObj pat_letters = pat_lowercases | pat_uppercases;

    // all the c tokens
    add_tokenizer("int", INT);
    add_tokenizer("main", MAIN);
    add_tokenizer("void", VOID);
    add_tokenizer("break", BREAK);
    add_tokenizer("do", DO);
    add_tokenizer("else", ELSE);
    add_tokenizer("if", IF);
    add_tokenizer("while", WHILE);
    add_tokenizer("return", RETURN);
    add_tokenizer("scanf", READ);
    add_tokenizer("printf", WRITE);
    add_tokenizer("{", LBRACE);
    add_tokenizer("}", RBRACE);
    add_tokenizer("[", LSQUARE);
    add_tokenizer("]", RSQUARE);
    add_tokenizer("(", LPAR);
    add_tokenizer(")", RPAR);
    add_tokenizer(";", SEMI);
    add_tokenizer("+", PLUS);
    add_tokenizer("-", MINUS);
    add_tokenizer("*", MUL_OP);
    add_tokenizer("/", DIV_OP);
    add_tokenizer("%", MOD_OP);
    add_tokenizer("&", AND_OP);
    add_tokenizer("|", OR_OP);
    add_tokenizer("!", NOT_OP);
    add_tokenizer("=", ASSIGN);
    add_tokenizer("<", LT);
    add_tokenizer(">", GT);
    add_tokenizer("<<", SHL_OP);
    add_tokenizer(">>", SHR_OP);
    add_tokenizer("==", EQ);
    add_tokenizer("!=", NOTEQ);
    add_tokenizer("<=", LTEQ);
    add_tokenizer(">=", GTEQ);
    add_tokenizer("&&", ANDAND);
    add_tokenizer("||", OROR);
    add_tokenizer(",", COMMA);

    // (digit)(digit)*
    add_tokenizer(
        Regex::RegexIter(pat_digits, 1) | \
            (RegexPatObj("0x") + Regex::RegexIter(pat_digits | RegexPatObj('a', 'f'), 1)),
        INT_NUM
    );

    // (letter)(digit|letter|_)*
    add_tokenizer(
        pat_letters + Regex::RegexIter(pat_digits | pat_letters | RegexPatObj("_"), 0),
        ID
    );

    end_pos = content.length();

}

// the longest prefix in the buffer
Token Scanner::next_token()
{
    // init
    int max_length = 0;
    Token ret_tok = { .token_type = NOTOK };

    while(ret_tok.token_type == NOTOK)
    {
        // iterate through all regexs
        for(const auto &item : regexs)
        {
            int matched_len = item.first.max_matched_lenghth(content.c_str() + begin_pos, end_pos - begin_pos);
            
            // update only if longer
            if(matched_len > max_length)
            {
                max_length = matched_len;
                ret_tok.token_type = item.second;
            }
        }

        if(ret_tok.token_type == NOTOK && empty())
        {
            ret_tok.token_type = END;
        }
        
        // ignore invalid input
        if(max_length == 0)
        {
            begin_pos += 1;
        }
        else
        {
            ret_tok.lexeme = content.substr(begin_pos, max_length);
        }

        begin_pos += max_length;
    }
    return ret_tok;
}


bool Scanner::empty()
{
    return end_pos - begin_pos <= 0;
}

}