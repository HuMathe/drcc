#ifndef DRCC_SCANNER_H
#define DRCC_SCANNER_H

#include "tokenType.h"
#include "regex/regex.h"


namespace DRCC
{

/// @brief C scanner with a binded stream and a buffer
///        this project is designed to identift tokens with length 
///        no longer than `MAX_TOKEN` and `MAX_BUFFER`, if 256 is not enough, please
///        modify the following `MAX_TOKEN` section
class Scanner
{
private:
    /// @brief input content
    std::string content;

    int begin_pos = 0;
    int end_pos = 0;

    /// @brief list of regex, identifying each token type
    std::vector<std::pair<Regex::RegexPatObj, TokenType>> regexs;

private:

    /// @brief initialization
    void init();

    /// @brief add a new regex representing a token type 
    void add_tokenizer(const char * const pat, TokenType token_type);
    void add_tokenizer(Regex::RegexPatObj regex, TokenType token_type);

public:
    Scanner();
    Scanner(const std::string & input);
    Scanner(std::istream &is);
    
    /// @brief the next token from the stream 
    Token next_token();

    /// @brief if there is no tokens left
    /// @return `true` if both the stream and the buffer exhausted
    bool empty();
};

}


#endif