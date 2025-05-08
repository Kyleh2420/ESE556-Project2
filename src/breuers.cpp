#include <vector>

#include "shared_variables.h"
#include "Node.h"
#include "Net.h"
#include "TreeNode.h"
#include "fm.cpp"

int leftArea = 0; 
int rightArea = 0; 
int totalArea = 0; 


void bisection() {

}

void quadrature(vector<Node>* Nodes, vector<Net>* Nets, TreeNode* currentNode){  // root node should be made and sent to this function. 
    if(Nodes->size()== 1){// Only one node remaining so we're at the leaf. 
        currentNode -> setNodeId((*Nodes)[0].getID()); 

        return; // you can't partition anymore. 
    }
    if (Nodes -> size() == 0) {return;}
    // Still need to add the xhigh, xlow, y high, ylow. These should be set when we set the root node and the we can use the area splits to get 
    // the new values for left and right children depending on the cutdirection of the current node. 
    // Also need to do the height and width thing but I think once we know the bounds for x and y then it should just be yhigh - y low and xhigh - xlow.

    int xh,xl,yh,yl;
    int xPartition, yPartition; // the x and y values that the cut goes through  
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

    yh = currentNode -> getYhigh(); 
    yl = currentNode -> getYlow(); 
    xh = currentNode -> getXhigh(); 
    xl = currentNode -> getXlow(); 

    if (currentNode -> getCutDirection() == 0) { // x high and x low are going to change for the children. Y high and y low are the same. 
        
        xPartition = ((float)(leftArea/totalArea) *(xh - xl)) + xl; 

        TreeNode* leftChild = new TreeNode(currentNode, xPartition, xlow, yh, yl); // x and y values are assigned for these. 
        TreeNode* rightChild = new TreeNode(currentNode, xh, xPartition, yh, yl);  
    }
    else{
        // left is the same as down 
        yPartition = ((float)(leftArea/totalArea) *(yh - yl)) + yl; 

        TreeNode* leftChild = new TreeNode(currentNode, xh, xl, yPartition, yl); // x and y values are assigned for these. 
        TreeNode* rightChild = new TreeNode(currentNode, xh,xl,yh, yPartition); // left is the same as down and right is the same as up.  
    }


    

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