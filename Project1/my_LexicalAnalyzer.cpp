/*
* Written By Nicholas Abate 
*/

/*
 * This .cpp file implements three structs and one class. The class my_LexicalAnalyzer
 * is used to implement this method my_GetToken. This method is the main functionality 
 * of this project. It will take in three arguemnts. A vector of reg structs,
 * An INPUT_TEXT, and the position of the last consumed character. 
 * The structs are ID_Info which contains the information such as ID name 
 * and position that ID is taken in, in order to do the semantics errors.
 * The other structs are the Nodes for the implementation of the REG struct. 
 * Node has its first and second label, inputed char to get to the next node,
 * and its first and second neighbor which is the next Node that this node can go
 * to if the that label is read. The REG struct is two nodes, the start Node and
 * the accept Node. It will containt the whole Node system using the linked list idea
 * of the Node points to the next Node. 
 * Furthermore: 
 * This class implements the semantic error help methods to make the semantic errors
*/

#include "my_LexicalAnalyzer.h"
#include <iostream>
#include <cstdlib>

using namespace std;

/*
 * Methods for REG_Nodes. Allows for nodes to point to other nodes.
 * Also have methods to check and set if the node is an accept node.
*/

//Takes in node pointer and updates current nodes link1 to point to argument
void REG_Node::attachNodeLink1(REG_Node* node) {
    nodeLink1 = node;
}

//Takes in node pointer and updates current nodes link2 to point to argument
void REG_Node::attachNodeLink2(REG_Node* node) {
    nodeLink2 = node;
}

//Returns bool value of stored data if the REG Node is accept state node
bool REG_Node::checkAccpetNode() {
    return isAcceptNode;
}

//Takes in a bool value and updates data value in REG_Node if node is accept state
void REG_Node::setAcceptNode(bool isAccept) {
    isAcceptNode = isAccept;
}

/*
 * Methods for REG. These methods allow for setting assigning REG to the 
 * correct start node and the correct accept node. 
*/

void REG::attachStartNode(REG_Node* node) {
    startNode = node;
}

void REG::attachAcceptNode(REG_Node* node) {
    acceptNode = node;
}

/*
 * These next few methods are for the REG and REG_Node logic. The match method
 * implements the REG and REG_Node logic and match_char implements match mehtod.
*/

//Method to combine REG structs for DOT rule
REG my_LexicalAnalyzer::combineREG_DOT(REG& reg1, REG& reg2) {

    //set REG1 accept node to point to REG2 start node 
    reg1.acceptNode->setAcceptNode(false);

    //remove accept state value from reg1 accept node
    reg1.acceptNode->attachNodeLink1(reg2.startNode);

    //set label for old accept Node to "_" so it can reach REG2 
    reg1.acceptNode->label1 = "_";

    return reg1;
}

//Method to combine REG structs for OR rule 
REG my_LexicalAnalyzer::combineREG_OR(REG& reg1, REG& reg2) {
    
    //create a new start and accept node and new REG
    REG_Node* newStart = new REG_Node();
    REG_Node* newAccept = new REG_Node();
    REG newReg;

    //make new start and accept node for new REG newStart and newAccept
    newReg.attachStartNode(newStart);
    newReg.attachAcceptNode(newAccept);
    newReg.acceptNode->setAcceptNode(true);

    //have new start node point to old start nodes
    newReg.startNode->label1 = "_";
    newReg.startNode->label2 = "_";
    newReg.startNode->attachNodeLink1(reg1.startNode);
    newReg.startNode->attachNodeLink2(reg2.startNode);

    //have old accept nodes point to new acceptNode
    //also remove accept status from old acceptNodes
    reg1.acceptNode->setAcceptNode(false);
    reg2.acceptNode->setAcceptNode(false);

    reg1.acceptNode->label1 = "_";
    reg1.acceptNode->attachNodeLink1(newReg.acceptNode);

    reg2.acceptNode->label1 = "_";
    reg2.acceptNode->attachNodeLink1(newReg.acceptNode);

    return newReg;
}

//Method to add * rule to reg argument
REG my_LexicalAnalyzer::combineREG_STAR(REG& reg) {
    
    //create a new accept and new start node 
    REG_Node* newStart = new REG_Node();
    REG_Node* newAccept = new REG_Node();
    newAccept->setAcceptNode(true); //make new accept node accept value

    //give new start node "_" label and point it to old start node
    newStart->attachNodeLink1(reg.acceptNode);
    newStart->label1 = "_";
    //also have it go to new accept node with label "_"
    newStart->attachNodeLink2(newAccept);
    newStart->label2 = "_";

    //set old accept to point to new old start with lable "_"
    //also make it no longer an accept node
    reg.acceptNode->label1 = "_";
    reg.acceptNode->attachNodeLink1(reg.startNode);
    reg.acceptNode->setAcceptNode(false);

    //set old accept node to go to new accpet node
    reg.acceptNode->label2 = "_";
    reg.acceptNode->attachNodeLink2(newAccept);

    //now make a new REG and give it new start node and accept node 
    REG newReg;
    newReg.attachStartNode(newStart);
    newReg.attachAcceptNode(newAccept);

    return newReg;

}

//This method takes in a label name for a CHAR and makes the corresponding REG
REG my_LexicalAnalyzer::createREG_CHAR(string labelName) {
    REG newReg;

    //Make startNode and acceptNode for REG
    REG_Node* start = new REG_Node();
    REG_Node* accept = new REG_Node();

    //make acceptNode accpet
    accept->setAcceptNode(true);

    //make start node go to accept node and give label
    start->attachNodeLink1(accept);
    start->label1 = labelName;

    //attach Nodes to REG
    newReg.attachStartNode(start);
    newReg.attachAcceptNode(accept);

    return newReg;

}

//This method creates a reg for UNDERSCORE and returns it
REG my_LexicalAnalyzer::createREG_UNDERSCORE() {
    REG newReg;

    //create nodes for REG
    REG_Node* start = new REG_Node();
    REG_Node* accept = new REG_Node();

    //make acceptNode accept
    accept->setAcceptNode(true);

    //make start node go to accept node label is always "_"
    start->attachNodeLink1(accept);
    start->label1 = "_";

    //attach Nodes to REG
    newReg.attachStartNode(start);
    newReg.attachAcceptNode(accept);

    return newReg;

}

vector<REG_Node*> my_LexicalAnalyzer::match_one_char(vector<REG_Node*> nodes, string s, int position) {
    char currentChar = s[position]; //char to match
    vector<REG_Node*> reachableNodes;

    for(REG_Node* node : nodes) {
        if(node->label1 == string(1, currentChar) && find(reachableNodes.begin(), reachableNodes.end(), node->nodeLink1) == reachableNodes.end()) {
            reachableNodes.push_back(node->nodeLink1);
        }

        if(node->label2 == string(1, currentChar) && find(reachableNodes.begin(), reachableNodes.end(), node->nodeLink2) == reachableNodes.end()) {
            reachableNodes.push_back(node->nodeLink2);
        }
    }

    if(reachableNodes.empty()) {
        return {};
    }

    return reachable_through_epsilon(reachableNodes);

}

int my_LexicalAnalyzer::match(REG reg, string input, int startPosition) {
    vector<REG_Node*> currentNodes = reachable_through_epsilon({reg.startNode});
    int matchedLength = 0;
    int longestMatchIndex = -1;

    while(!currentNodes.empty() && startPosition < input.size()) {
        char currentChar = input[startPosition];
        currentNodes = match_one_char(currentNodes, input, startPosition);

        if(!currentNodes.empty()) {
            startPosition++;
            matchedLength++;
        }

        for(REG_Node* node : currentNodes) {
            if(node->checkAccpetNode()) {
                longestMatchIndex = startPosition;
            }
        }
    }

    return (longestMatchIndex > 0) ? (longestMatchIndex - matchedLength) : -1;
}

void my_LexicalAnalyzer::my_GetToken(string inputText) {
    int longestMatchLength = -1;
    int longestMatchRegIndex = -1;
    string longestMatchedString = "";

    size_t nonWhitespacePos = inputText.find_first_not_of(" \t\n");
    if(nonWhitespacePos != string::npos) {
        inputText = inputText.substr(nonWhitespacePos);
    }

    if(inputText.empty()) {
        cout << "ERROR" << endl;
        return;
    }

    for(int i = 0; i < REG_Vector.size(); ++i) {
        REG currentReg = REG_Vector[i];
        int matchStartPos = 0;
        int matchResult = match(currentReg, inputText, matchStartPos);

        if(matchResult != -1) {
            int matchedLength = matchStartPos - matchResult;

            if(matchedLength > longestMatchLength) {
                longestMatchLength = matchedLength;
                longestMatchRegIndex = i;
                longestMatchedString = inputText.substr(matchResult, matchedLength);
            }
        }
    }

    if(longestMatchRegIndex != -1) {
       cout << idList[longestMatchRegIndex].name << ", \"" << longestMatchedString << "\"" << endl;
    }
    else {
        cout << "ERROR" << endl;
    }
}

/*
 * This method returns the set of vectors that is reachable through the 
 * transition of "_" in the label of the node. It will take in a vector 
 * of nodes and return a vector of nodes being the subset of inputed Nodes 
 * that have the "_" transition. It will allow for match_one_char to work 
*/

vector<REG_Node*> my_LexicalAnalyzer::reachable_through_epsilon(vector<REG_Node*> nodes) {
    vector<REG_Node*> reachableNodes;
    vector<REG_Node*> tempNodes;
    bool changed = true;

    while(changed) {
        changed = false;

        for(REG_Node* node : nodes) {

            if(node->label1 == "_" && find(tempNodes.begin(), tempNodes.end(), node->nodeLink1) == tempNodes.end()) {
                tempNodes.push_back(node->nodeLink2);
            }

            if(node->label2 == "_" && find(tempNodes.begin(), tempNodes.end(), node->nodeLink2) == tempNodes.end()) {
                tempNodes.push_back(node->nodeLink2);
            }
        }

        if(tempNodes != reachableNodes) {
            changed = true;
            reachableNodes = tempNodes;
        }
    }

    return reachableNodes;
}

/*
 * REG notes: A REG will be created when parse_expr() is called.
 * But in some situations this REG will need to be motified with another
 * REG due to the rules in the grammar. Every time there is a LPAREN there
 * needs to be a modification done to the REG. Therefore everytime LPAREN is
 * read as a token there needs to be a new REG made for it. Then when the
 * RPAREN is read the REG is done being made. At most two REGs are going to 
 * be combined into a single REG. There is two REGs created if and only if
 * there is a DOT or an OR read. This combination will be done at the end of 
 * reading DOT or OR in order to get both REGs and combine them into one. 
 * CHAR has an interesting rule in the grammar. Using this a REG is made 
 * when CHAR is consumed same with UNDERSCORE.  
*/



/*
* This method returns a boolean. It takes in a string name and a line number
* to check if to add it can add it to the vector of structs called IDs. 
* If there is no match found of ID already in the vector it returns true.
* If there was a match of an ID already being found in the vector it returns false
*/
bool my_LexicalAnalyzer::addID(string name, int line) {
    if(checkForID(name) == -1) { //checkForID will return -1 if there is no match
        ID_Info newID = {name, line};
        idList.push_back(newID);
        return true; //no match found ID added to idList
    }
    else {
        return false; //there was a match found 
    }
}

/*
* This method takes in a string name and uses that to find if there
* exists an id in the idList that has the same name. It does this by 
* checking the contents of the idList to see if one of the ids has 
* the same name. It returns the line number of the matched name, of the 
* original ID in the list and returns -1 if there is no match 
*/
int my_LexicalAnalyzer::checkForID(string name) {
    for(ID_Info &id : idList) {
        if(id.name == name) {
            return id.lineNum;
        }
    }
    return -1; //No duplicate found
}