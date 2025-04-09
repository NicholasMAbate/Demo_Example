/*
* Written by Nicholas Abate
*/

/*
* This file will contain the class epsilon helper. It will help 
* parser.cpp determine if there will be a epsilon generated from the given
* or inputed grammar rule. This is done by the following method. 
* Epsilon can be generaterd through UNDERSCORE. Through, STAR
* as STAR can generate the empty string. OR so long as either side 
* is already set as true for genarating epsilon. Concatination so long
* as both sides can generate epsilon. 
*/

#include "epsilon.h"
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

/*
 * This class will serve as a helper for determining if there is 
 * a possible epsilon generation from the inputed grammar rule. 
 * It will also help determine if there needs to be a output statement 
 * for the epsilon generation.
*/

/*
 * This method adds an Epsilon node to the vector of nodes, using 
 * its token name and its token type to understand if it is a 
 * STAR OR DOT
 * returns true when node was added to the vector 
*/
bool EpsilonGeneratorHelper::addEpsilonNode(string name) {
    
    for(epsilonNode &node : epsilonNodeList) {
        if(node.name == name) {
            return false; //Node is already in list (should not happen but just in case)
        }
    }

    //when added we dont know any information yet so always initalized to FALSE 
    bool canGenerate = false;

    epsilonNodeList.push_back({name, canGenerate}); //add new node to vector
    
    return true; //Node is added into the vector
}

/*
 * This method updates the node in the vector list using its ID name 
 * to update if the node will generate an epsilon. Returns nothing.
*/
void EpsilonGeneratorHelper::updateNodeValue(string name, bool generates) {
    
    for(epsilonNode &node : epsilonNodeList) {
        if(node.name == name) {
            node.canGenerate = generates;
            return;
        }
    }

    //if node is not found add it (should never run but just incase :D )
    epsilonNodeList.push_back({name, generates});
}

/*
 * This method returns the value of the node, if it is TRUE or FALSE
 * TRUE meaning the regular expression (so far) does generate epsilon 
 * FALSE meaning the regular epxpression (so far) can not generate epsilon
*/
bool EpsilonGeneratorHelper::getEpsilonNodeValue(string name) {

    for(epsilonNode &node : epsilonNodeList) {
        if(node.name == name) {
            return node.canGenerate;
        }
    }

    return false; //if node is not found therefore it has to be false
}

/*
 * This method is to help printout the error message. It will print
 * the name of the nodes in this way 
 * Error message !!! token_name1 token_namei
*/
void EpsilonGeneratorHelper::printNodeNames() {
    
    bool epsilonErrorFound = false;
    for(epsilonNode &node : epsilonNodeList) {
        if (node.canGenerate) {
            epsilonErrorFound = true;
        }
    }

    if(epsilonErrorFound) {
        cout << "EPSILON IS NOOOOOOOT A TOKEN !!! ";
    }
    

    //print names in order of first to last in input.txt file 
    for(epsilonNode &node : epsilonNodeList) {
        if(node.canGenerate) {
            cout << node.name << " ";
        }
    }

    cout << "\n";
}


/* 
 * Additional Notes:
 * A node will be added right before the ID is parsed
 * A node will be done updating once the SEMICOLON or HASH is read
 * The printout of the epsilon generated IDs will be printed once the EOF
 * is consumed/read
 * Update will be called every time a expr is parsed changing the rules 
 * of how the epsilon generation works. 
*/
