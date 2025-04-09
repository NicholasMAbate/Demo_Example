/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <vector> 
#include "lexer.h"
#include "my_LexicalAnalyzer.h"
#include "epsilon.h"
#include "inputbuf.h"

class Parser {
  public:
    void parse_input();
    void readAndPrintAllInput();
    LexicalAnalyzer lexer;
    my_LexicalAnalyzer my_lexer; // I added this class
    EpsilonGeneratorHelper epsilonHelper; //I added this class
    void syntax_error();
    void syntax_error_expr(std::string lexeme); //I added this method
    Token expect(TokenType expected_type);
    Token expect_expr(TokenType expected_type, std::string ID); //I added this method 
    void parse_tokens_section();
    void parse_token_list();
    void parse_token();
    REG parse_expr(std::string lexeme); //I added a argument so it can catch syntax error also added return type of struct REG
    std::vector<REG*> REG_vector; //Vector of all REGs, one per ID
    std::string inputText;
    std::string getInputText();
};

#endif

