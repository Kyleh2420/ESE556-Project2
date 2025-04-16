#include <vector>

#include "shared_variables.h"
#include "Node.h"
#include "Net.h"
#include "TreeNode.h"
#include "fiducciaMattheyses.cpp"


void bisection() {

}

void quadrature(vector<Node>* Nodes, vector<Net>* Nets, TreeNode* currentNode){  // root node should be made and sent to this function. 
    if(Nodes->size()== 1){// Only one node remaining so we're at the leaf. 
        currentNode -> setNodeId((*Nodes)[0].getID()); 

        return; // you can't partition anymore. 
    }
    // Still need to add the xhigh, xlow, y high, ylow. These should be set when we set the root node and the we can use the area splits to get 
    // the new values for left and right children depending on the cutdirection of the current node. 
    // Also need to do the height and width thing but I think once we know the bounds for x and y then it should just be yhigh - y low and xhigh - xlow.

    int lastCut = FM(Nodes, Nets); 
    vector<Node> leftNodes; 
    vector<Node> rightNodes; 
    
    for (int i = 0; i < (*Nodes).size(); i++){
        if((*Nodes)[i].whichPartition() == 1){
            rightNodes.push_back((*Nodes)[i]); 
        }
        else {
            leftNodes.push_back((*Nodes)[i]); 
        }
    }

    TreeNode* leftChild = new TreeNode(); // x and y values are assigned for these. 
    TreeNode* rightChild = new TreeNode(); 

    currentNode -> setLeftChild(leftChild); 
    currentNode -> setRightChild(rightChild); 
    leftChild -> setParent(currentNode); 
    rightChild -> setParent(currentNode); 

    bool flipDirection = !(currentNode -> getCutDirection()); 
    leftChild -> setCutDirection(flipDirection); 
    rightChild -> setCutDirection(flipDirection);
    // set the values of the attributes of treenode. 
    
    quadrature(&leftNodes, Nets, leftChild); 
    quadrature(&rightNodes, Nets, rightChild); 
}

void sliceBisection() {

}

void cutOriented() {
    
}