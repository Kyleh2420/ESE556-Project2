#include <vector>

#include "shared_variables.h"
#include "Node.h"
#include "Net.h"
#include "TreeNode.h"
#include "fm.cpp"
// 
// int maxWidth = 0;
// int leftArea = 0;
// int rightArea = 0;
// int totalArea = 0;
// 
/* 
    naming convention for cut partitioning:
    0 = left / down
    1 = right / up
*/

void bisection(vector<Node>* Nodes, vector<Net>* Nets, TreeNode* currentNode) {
    if(Nodes->size() == 1) { // Only one node remaining so we're at the leaf.
        currentNode->setNodeId((*Nodes)[0].getID());
        currentNode->setLeftChild(nullptr);
        currentNode->setRightChild(nullptr);
        return; // you can't partition anymore.
    }
    if(Nodes->size() == 0) { return; } // No nodes to partition.
    
    bool cutDir = currentNode->getCutDirection();
    float ratio = 0.0;
    if (ratio == maxWidth)
        cutDir = 1; // Flip cut direction if ratio is maxWidth

    int lastCut = FM(*Nodes, *Nets, numNodes);

    int xh,xl,yh,yl;
    int xPartition, yPartition; // the x and y values that the cut goes through  
    
    vector<Node> leftNodes;
    vector<Node> rightNodes;

    TreeNode* leftChild;
    TreeNode* rightChild;
    for (int i = 0; i < (*Nodes).size(); i++) {
        if((*Nodes)[i].whichPartition() == 1)
            rightNodes.push_back((*Nodes)[i]);
        else 
            leftNodes.push_back((*Nodes)[i]);
    }

    yh = currentNode -> getYhigh(); 
    yl = currentNode -> getYlow(); 
    xh = currentNode -> getXhigh(); 
    xl = currentNode -> getXlow(); 

    if (currentNode -> getCutDirection() == 0) { // x high and x low are going to change for the children. Y high and y low are the same. 
        
        xPartition = ((float)(leftArea/totalArea) *(xh - xl)) + xl; 

        leftChild = new TreeNode( currentNode, xPartition, xl, yh, yl); // x and y values are assigned for these. 
        rightChild = new TreeNode(currentNode, xh, xPartition, yh, yl);  
    }
    else{
        // left is the same as down 
        yPartition = ((float)(leftArea/totalArea) *(yh - yl)) + yl; 

        leftChild = new TreeNode(currentNode, xh, xl, yPartition, yl); // x and y values are assigned for these. 
        rightChild = new TreeNode(currentNode, xh, xl, yh, yPartition); // left is the same as down and right is the same as up.  
    }

    currentNode -> setLeftChild(leftChild);
    currentNode -> setRightChild(rightChild);

    leftChild -> setParent(currentNode); 
    rightChild -> setParent(currentNode); 

    bisection(&leftNodes, Nets, currentNode->getLeftChild());
    bisection(&rightNodes, Nets, currentNode->getRightChild());
}

void quadrature(vector<Node>* Nodes, vector<Net>* Nets, TreeNode* currentNode){  // root node should be made and sent to this function. 
    if(Nodes->size()== 1){// Only one node remaining so we're at the leaf. 
        currentNode -> setNodeId((*Nodes)[0].getID()); 
        currentNode->setLeftChild(nullptr);
        currentNode->setRightChild(nullptr);
        return; // you can't partition anymore. 
    }
    if (Nodes -> size() == 0) {return;}
    // Still need to add the xhigh, xlow, y high, ylow. These should be set when we set the root node and the we can use the area splits to get 
    // the new values for left and right children depending on the cutdirection of the current node. 
    // Also need to do the height and width thing but I think once we know the bounds for x and y then it should just be yhigh - y low and xhigh - xlow.
    cout << "test1" << endl;
    int xh,xl,yh,yl;
    int xPartition, yPartition; // the x and y values that the cut goes through  
    int lastCut = FM(*Nodes, *Nets, numNodes); // this is the cutsize.
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
    cout << "test2" << endl;
    yh = currentNode -> getYhigh(); 
    yl = currentNode -> getYlow(); 
    xh = currentNode -> getXhigh(); 
    xl = currentNode -> getXlow(); 

    TreeNode* leftChild;
    TreeNode* rightChild;

    if (currentNode -> getCutDirection() == 0) { // x high and x low are going to change for the children. Y high and y low are the same. 
        
        xPartition = ((float)(leftArea/totalArea) *(xh - xl)) + xl; 

        leftChild = new TreeNode( currentNode, xPartition, xl, yh, yl); // x and y values are assigned for these. 
        rightChild = new TreeNode(currentNode, xh, xPartition, yh, yl);  
    }
    else{
        // left is the same as down 
        yPartition = ((float)(leftArea/totalArea) *(yh - yl)) + yl; 

        leftChild = new TreeNode(currentNode, xh, xl, yPartition, yl); // x and y values are assigned for these. 
        rightChild = new TreeNode(currentNode, xh, xl, yh, yPartition); // left is the same as down and right is the same as up.  
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

// Standard FM‐driven bisection, but always vertical cuts for columns
void columnBisection(std::vector<Node>* nodes,
                     std::vector<Net>*  nets,
                     TreeNode*         currentNode)
{
    int n = nodes->size();
    if (n <= 1) {
        if (n == 1)
            currentNode->setNodeId((*nodes)[0].getID());
        currentNode->setLeftChild(nullptr);
        currentNode->setRightChild(nullptr);
        return;
    }

    // bisect into two equal halves
    int target = n/2;
    int lastCut = FM(*nodes, *nets, target);

    std::vector<Node> leftNodes, rightNodes;
    for (auto &m : *nodes) {
        if (m.whichPartition() == 0) leftNodes.push_back(m);
        else                        rightNodes.push_back(m);
    }

    int xl = currentNode->getXlow();
    int xh = currentNode->getXhigh();
    int yl = currentNode->getYlow();
    int yh = currentNode->getYhigh();
    float frac = float(leftNodes.size()) / n;
    int xPartition = xl + int(frac * float(xh - xl) + 0.5f);

    TreeNode* leftChild  = new TreeNode(currentNode, xPartition, xl, yh, yl);
    TreeNode* rightChild = new TreeNode(currentNode, xh, xPartition, yh, yl);

    currentNode->setLeftChild (leftChild);
    currentNode->setRightChild(rightChild);
    leftChild ->setParent(currentNode);
    rightChild->setParent(currentNode);

    columnBisection(&leftNodes,  nets, leftChild);
    columnBisection(&rightNodes, nets, rightChild);
}

void slice(vector<Node>* Nodes, vector<Net>* Nets, TreeNode* currentNode, int K) {
    int n = Nodes->size();
    if (n == 0) return;
    if (n == 1) {
        // Leaf: single module assigned to this leaf cell
        currentNode->setNodeId((*Nodes)[0].getID());
        currentNode->setLeftChild(nullptr);
        currentNode->setRightChild(nullptr);
        return;
    }

    // If fewer than K remain, switch to vertical bisection on this slice
    if (n <= K) {
        columnBisection(Nodes, Nets, currentNode);
        return;
    }

    // 1) Perform FM min‐cut to extract exactly K modules
    int lastCut = FM(*Nodes, *Nets, K);

    // 2) Partition modules into slice (size K) and remainder (size n-K)
    std::vector<Node> sliceNodes, remNodes;
    for (auto &m : *Nodes) {
        if (m.whichPartition() == 0)
            sliceNodes.push_back(m);
        else
            remNodes.push_back(m);
    }

    // 3) Compute the Y‐coordinate of the horizontal cut
    int xl = currentNode->getXlow();
    int xh = currentNode->getXhigh();
    int yl = currentNode->getYlow();
    int yh = currentNode->getYhigh();
    float frac = float(K) / n;
    int yPartition = yl + int(frac * float(yh - yl) + 0.5f);

    // 4) Create two child regions: bottom slice & above‐slice remainder
    TreeNode* sliceChild = new TreeNode(currentNode, xh, xl, yPartition, yl);
    TreeNode* remChild   = new TreeNode(currentNode, xh, xl, yh,   yPartition);

    currentNode->setLeftChild (sliceChild);
    currentNode->setRightChild(remChild);
    sliceChild->setParent(currentNode);
    remChild  ->setParent(currentNode);

    // 5) For the bottom slice: assign columns by vertical bisection
    sliceChild->setCutDirection(0);  // force vertical cuts
    columnBisection(&sliceNodes, Nets, sliceChild);

    // 6) For the remainder: continue slicing horizontally
    remChild->setCutDirection(1);    // force horizontal cuts
    slice(&remNodes, Nets, remChild, K);
}

void cutOriented(vector<Node>* Nodes, vector<Net>* Nets, TreeNode* currentNode) {
    
}