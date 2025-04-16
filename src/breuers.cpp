#include <vector>

#include "shared_variables.h"
#include "Node.h"
#include "Net.h"
#include "TreeNode.h"


void bisection() {

}

void quadrature(vector<Node>* Nodes, vector<Net>* Nets){
    int lastCut = FM(&Nodes, &Nets); // this will also be returning the area split so I can use that do determine the x and y bounds
    vector<Node> leftNodes; 
    vector<Node> rightNodes; 
    vector<Net> leftNets; 
    vector<Net> rightNets; // Idk how to get these just yet. 
    for (int i = 0; i < (*Nodes).size(); i++){
        if((*Nodes)[i].whichPartition() == 1){
            rightNodes.push_back((*Nodes)[i]); 
        }
        else {
            leftNodes.push_back((*Nodes)[i]); 
        }
    }
    TreeNode* newTreeNode = new TreeNode();
    // set the values of the attributes of treenode. 
    // Figure out how to send the associated nets as well. 
    
    quadrature(&leftNodes, &leftNets); 
    quadrature(&rightNodes, &rightNets); 
}

void sliceBisection() {

}

void cutOriented() {
    
}