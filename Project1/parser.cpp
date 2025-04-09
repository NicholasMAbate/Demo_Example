/*Nicholas Abate
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>
#include "parser.h"
#include "my_LexicalAnalyzer.h"


using namespace std;


// this syntax error function needs to be
// modified to produce the appropriate message
void Parser::syntax_error()
{
    cout << "SNTYAX ERORR";
    exit(1);
}

//this syntax error functions ouputs 
//an error when in parse_expr()
void Parser::syntax_error_expr(string lexeme) {
    cout << lexeme << " HAS A SYNTAX ERROR IN ITS EXPRESSION";
    exit(1);
}


// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

/*
* This method is called in parse_expr and is used to 
* output the correct error message for when there is a 
* syntax error in the regualr expression parssing that 
* is caued by a mismatched token. Ex: |! rather than |(
*/
Token Parser::expect_expr(TokenType expected_type, string ID) {
    Token t = lexer.peek(1);
    if(t.token_type != expected_type) {
        syntax_error_expr(ID);
    }
    return t;
}


/*
* This method is called by parse_token. It returns nothing.
* Has many rules to go to, this is the lowest level of parse calls.
* Takes in argument called lexeme which is the name of the ID 
* of the parsing rule. 
*/
REG Parser::parse_expr(string lexeme)
{
    //cout << "Running parse_expr\n";

    //expr -> CHAR 
    //expr -> LPAREN expr RPAREN DOT LPAREN expr RPAREN
    //expr -> LPAREN expr RPAREN OR LPAREN expr RPAREN
    //expr -> LPAREN expr RPAREN STAR
    //expr -> UNDERSCORE  !!this is epsilon!! 

    Token t = lexer.peek(1); //peek to see next token / terminal 

    //Completed REG to return when done parsing
    REG parsingReg;
    REG reg1, reg2; //regs to preform combining reg operations 

    if(t.token_type == CHAR) { //if the token is char then end
        
        //cout << "expect CHAR\n";

        Token charToken = expect(CHAR); //consume the CHAR

        //Create a REG for this char rule 
        string labelName = charToken.lexeme;
        parsingReg = my_lexer.createREG_CHAR(labelName);

        //epsilon generator
        epsilonHelper.updateNodeValue(lexeme, false);

        return parsingReg;
    }

    else if(t.token_type == UNDERSCORE) {
        //cout << "expect UNDERSCORE\n";

        //main logic of parser
        expect(UNDERSCORE); //consume UNDERSCORE (epsilon);
        parsingReg = my_lexer.createREG_UNDERSCORE();

        //logic for epsilon helper
        epsilonHelper.updateNodeValue(lexeme, true);

        return parsingReg;
    }

    else if(t.token_type == LPAREN) { //two rules for seeing this terminal
        //cout << "expect LPAREN\n";

        expect(LPAREN); //consume LPAREN
        reg1 = parse_expr(lexeme); //reg1 for reg combination
        expect_expr(RPAREN, lexeme); //check to make sure next token is RPAREN
        expect(RPAREN); //consume always coming RPAREN

        t = lexer.peek(1); //has to be a DOT, OR, STAR
        
        //expr -> DOT LPAREN expr RPAREN
        if(t.token_type == DOT) {

            //main logic of parser with epsilon check aswell
            expect(DOT); //consume the DOT
            expect_expr(LPAREN, lexeme); //check to make sure next token is LPAREN
            expect(LPAREN); //This line and next two lines are always follow this
            
            //left side check of expression for epsilon error (before parse)
            bool leftSideEpsilon = epsilonHelper.getEpsilonNodeValue(lexeme); 

            reg2 = parse_expr(lexeme); //reg 2 for reg combination 

            //right side check of expression for epsilon error (after parse
            bool rightSideEpsilon = epsilonHelper.getEpsilonNodeValue(lexeme);
            
            expect_expr(RPAREN, lexeme); //check to make sure next token is RPAREN
            expect(RPAREN);

            //preform reg combination and put it in parsing Reg
            parsingReg = my_lexer.combineREG_DOT(reg1, reg2);

            //both side of DOT generate epsilon therefore epsilon is generated
            bool finalEpsilon = leftSideEpsilon && rightSideEpsilon;
            epsilonHelper.updateNodeValue(lexeme, finalEpsilon);
        }

        //expr -> LPAREN expr RPAREN 
        else if(t.token_type == OR) {


            //main logic of parser with epsilon check aswell
            expect(OR); //consume the OR
            expect_expr(LPAREN, lexeme); //check to make sure next token is LPAREN
            expect(LPAREN); //This line and next two lines are always follow this
            
            //check epsilon generation before expr parse 
            bool currentEpsilon = epsilonHelper.getEpsilonNodeValue(lexeme);
            
            reg2 = parse_expr(lexeme); //reg 2 for reg combination
            expect_expr(RPAREN, lexeme); //check to make sure next token is RPAREN
            expect(RPAREN);

            //combine regs and add it to parsing Reg
            parsingReg = my_lexer.combineREG_OR(reg1, reg2);

            //if the side before or after the parse is epsilon generatable then
            //whole expr is epsilon generatable 
            bool parsedEpsilon = epsilonHelper.getEpsilonNodeValue(lexeme);
            epsilonHelper.updateNodeValue(lexeme, currentEpsilon || parsedEpsilon);
        }

        //expr -> STAR
        else if(t.token_type == STAR) {

            expect(STAR); //consume the STAR
            
            //apply start operation to reg
            parsingReg = my_lexer.combineREG_STAR(reg1);

            //STAR wull always generates epsilon
            epsilonHelper.updateNodeValue(lexeme, true);

            return parsingReg;
        }

        //if none of the above rules syntax error
        else {
            syntax_error_expr(lexeme);
        }
        

    }

    //return syntax_error if no opptions apply
    else {
        syntax_error_expr(lexeme);
    }

    return parsingReg;
}


/*
* This Method is called by parse_token_list(). It returns nothing.
* Only has one rule.
* In this function there is also a portion that is 
* just focused on semantic checking to ensure that there is no repeated ID. 
*/
void Parser::parse_token()
{
    //cout << "running parse_token\n";

    //token -> ID expr
    //Peek and consuke ID to move on and parse expr

    Token t = lexer.peek(1);
    Token error_catch = lexer.peek(1); //this is used for syntax_error_expr
    Token epsilon_ID = lexer.peek(1); //this is used for the epsilon helper class

    /*
     * This next line is for the epsilon helper class where it adds the ID
     * to the epsilonNode list, should initalize node value to FALSE
    */
    
    epsilonHelper.addEpsilonNode(epsilon_ID.lexeme);

    /*
     * This block is used for checking if the peeked token is an ID 
     * And if it is consume the token and contiue the parse 
     * Also do the semantics checks 
     */
    if(t.token_type == ID ) {
        //cout << "expecting ID\n";

        /*
        * Semantic checking
        * First get line number then if it is not duplicate 
        * add it to the idList
        */
        int duplicateCheck = my_lexer.checkForID(error_catch.lexeme);
        if(duplicateCheck != -1) {
            cout << "Line " << error_catch.line_no << ": " << error_catch.lexeme
            << " already declared on line " << duplicateCheck << "\n";
        }
        else {
            my_lexer.addID(error_catch.lexeme, error_catch.line_no);
        }

        t = expect(ID); //Consume ID to move on to expr
      
        REG*  bigREG = new REG(parse_expr(error_catch.lexeme)); //now parse expr 
        //error_catch.lexeme is used as argument to know what ID parse_expr is taking care of

        //add finished reg "bigREG" to vector of other finished REGs
        REG_vector.push_back(bigREG);
        
    }

    //if the next Token is not an ID then return syntax_error
    else {
        syntax_error();
    }

    //The ID and expr parsed has the same vector index in their respective 
    //vectors due to them being pushed at the "same time" Therefore 
    //if I need to know the matching ID to REG I just check index of REG
    //In the REG_vector and match it to the id_list vector
}


/*
* This method is called by parse_tokens_section(). It returns nothing.
* Has two rules. Could call itself.
*/
void Parser::parse_token_list()
{
    //cout << "running parse_token_list\n";
    //token_list -> token
    //token_list -> token SEMICOLON token_list

    parse_token(); //all rules start with token 

    //peek to see if SEMICOLON rule
    Token t = lexer.peek(1);
    if(t.token_type == SEMICOLON) {
        //CONSUME SEMICOLON
        //cout << "expecting SEMICOLON\n";
        expect(SEMICOLON);

        parse_token_list(); //parse the next token_list
    }
    //If it is not a SEMICOLON then token list is over and should turn into a HASH
    else if(t.token_type == HASH) {
        return;
    }
    //return syntax_error if neither 
    else {
        syntax_error();
    }
}


/*
* This method is called by parse_input(). It returns nothing.
* Will parse some token list then will see hash then exit.
*/
void Parser::parse_tokens_section()
{
    //cout << "running parse_tokens_section\n";

    //tokens_section -> token_list HASH
    parse_token_list();

    //cout << "expecting HASH\n";

    expect(HASH);
}


/*
* This is the first parse called. It is the only method called by main.
* This method parses the inputed .txt file
*/
void Parser::parse_input()
{
    //cout << "running parse_input\n";

    //input -> tokens_section INPUT_TEXT
    parse_tokens_section();

    //cout << "expecting INPUT_TEXT";

    Token inputTextToken = expect(INPUT_TEXT);

    //store input text in string 
    inputText = inputTextToken.lexeme;

    expect(END_OF_FILE);

    //once done parsing check if epsilon error happens
    epsilonHelper.printNodeNames();
    //my_lexer.my_GetToken(inputText);

}


// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this function does
// This function is not needed for your solution and it is only provided to
// illustrate the basic functionality of getToken() and the Token type.


void Parser::readAndPrintAllInput()
{
    Token t;


    // get a token
    t = lexer.GetToken();


    // while end of input is not reached
    while (t.token_type != END_OF_FILE)
    {
        t.Print();          // pringt token
        t = lexer.GetToken();   // and get another one
    }
       
    // note that you should use END_OF_FILE and not EOF
}

string Parser::getInputText() {
    return inputText;
}

int main()
{
    // note: the parser class has a lexer object instantiated in it (see file
    // parser.h). You should not be declaring a separate lexer object.
    // You can access the lexer object in the parser functions as shown in
    // the example  method Parser::readAndPrintAllInput()
    // If you declare another lexer object, lexical analysis will
    // not work correctly
    Parser parser; //Make the parser object


    parser.parse_input(); //The call to this function should parse the entire input through recursive descent parsing
    //string inputText = parser.getInputText();
    //parser.my_lexer.my_GetToken(inputText);
    //parser.readAndPrintAllInput(); //this is a test print that prints all of the input into the out
   
}
