/*
* Written by Nicholas Abate
* More explination on the .cpp file 
*/

#ifndef EPSILON__H
#define EPSILON__H

//include everything just in case lol 
#include <string>
#include <vector>


//struct holds if the expr can generate epsilon and its name 
struct epsilonNode {
    std::string name;
    bool canGenerate;
};

//helpper class to ensure that Epsilon error is handled. 
class EpsilonGeneratorHelper {

    public:
    bool addEpsilonNode(std::string name); //for updating if the given name can generate epsilon
    void updateNodeValue(std::string name, bool generates); //for adding a new Node to the list
    bool getEpsilonNodeValue(std::string name); //for getting node value if it can generate.
    void printNodeNames(); //prints out all of the names of the Epsilon generating nodes
    
    private:
    std::vector<epsilonNode> epsilonNodeList; //vector of epsilon Nodes 

};

#endif