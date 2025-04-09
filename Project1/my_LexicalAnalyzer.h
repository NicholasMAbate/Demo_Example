/*
* Written by Nicholas Abate
*/

//More notes on impelentation on .cpp file 

#ifndef __MY_LEXICALANALYZER__H__
#define __MY_LEXICALANALYZER__H__

#include <string>
#include <vector>
#include <stack>
#include <set>
#include <algorithm>

struct ID_Info {
    std::string name;
    int lineNum;
};

/*
 * REG_Node struct to make REG struct and regular expression parsing work
 * Contains label1 name and label2 name, then the pointer to the next node
 * Idea: if the correct label is inputed/read then go to the matching link
*/

struct REG_Node {

    //Node names and Node pointers to neighboring Nodes 
    std::string label1;
    std::string label2;
    REG_Node* nodeLink1;
    REG_Node* nodeLink2;
    bool isAcceptNode;

    //constructor to initalize labels and set links to nullptr
    REG_Node() : label1("NULL"), label2("NULL"), nodeLink1(nullptr), nodeLink2(nullptr) {}
    
    //method to take in node pointer and attach it to link1
    void attachNodeLink1(REG_Node* node);

    //method to take in node pointer and set link to link2
    void attachNodeLink2(REG_Node* node);

    //method to check if this node is an accept node 
    bool checkAccpetNode();

    //method to set if this node is accept node
    void setAcceptNode(bool isAccept);

};

/*
 * REG struct containts the pointers to the first Node in the regular expression
 * and the accept Node in the regular expression. 
 * Contains methods to set the pointer of the start node and accept node
 * Has a constructor method which initalizes node pointers to nullptr
 * Idea: Call REG struct to match method with a character, then check
 * if it is able to accept the character 
*/

struct REG {
    public:

    REG_Node* startNode;
    REG_Node* acceptNode;

    //constructor
    REG() : startNode(nullptr), acceptNode(nullptr) {}

    //update pointer values
    void attachStartNode(REG_Node* node);
    void attachAcceptNode(REG_Node* node);

};



/*
 * Class implements sematic errors and logic to consume input 
*/
class my_LexicalAnalyzer{
    public:
    int NODE_COUNTER = 0; //int to keep track of Node names 
    bool addID(std::string name, int line); //adding a new ID 
    int checkForID(std::string name); //checking if ID is already taken

     /* Takes in a REG struct address and a char, goes through the
     * REG_Nodes in the REG struct to see if the char can traverse 
     * Through nodes. Returns a pointer to a vector of nodes in which
     * the char can go through else returns an empty vector */
    std::vector<REG_Node*> match_one_char(REG& reg, char c);
    REG combineREG_DOT(REG& reg1, REG& reg2); // method for combining REGs for DOT
    REG combineREG_OR(REG& reg1, REG& reg2); //method for combining REGs for OR
    REG combineREG_STAR(REG& reg); //method for STAR rule 
    REG createREG_CHAR(std::string labelName); //method to create REG for CHAR
    REG createREG_UNDERSCORE(); //method to create REG for underscore 
    int match(REG reg, std::string inputText, int position); //method for match 
    std::vector<REG_Node*> match_one_char(std::vector<REG_Node*> nodeVector, std::string s, int position); //method to match one char
    void my_GetToken(std::string inputText); //final method that does everything :D 
    std::vector<REG_Node*> reachable_through_epsilon(std::vector<REG_Node*> nodes);

    private:
    std::vector<ID_Info> idList; //vector of ID structures
    std::vector<REG_Node> REG_Nodes; //vector of REG_Nodes
    std::vector<REG> REG_Vector; //vector of REGs 

};

#endif