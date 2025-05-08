#include <vector>
#include <cmath>

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
    
    int lastCut = FM(*Nodes, *Nets, Nodes->size());

    int xh,xl,yh,yl;
    int xPartition, yPartition; // the x and y values that the cut goes through  
    
    vector<Node> leftNodes;
    vector<Node> rightNodes;

    TreeNode* leftChild;
    TreeNode* rightChild;

    int localLeftArea = 0;
    int localRightArea = 0;
    int localTotalArea = 0;
    for (int i = 0; i < (*Nodes).size(); i++){
        if((*Nodes)[i].whichPartition() == 1){
            rightNodes.push_back((*Nodes)[i]); 
            localRightArea += (*Nodes)[i].getArea();
        }
        else {
            leftNodes.push_back((*Nodes)[i]);
            localLeftArea += (*Nodes)[i].getArea();
        }
        localTotalArea += (*Nodes)[i].getArea();
    }

    yh = currentNode -> getYhigh(); 
    yl = currentNode -> getYlow(); 
    xh = currentNode -> getXhigh(); 
    xl = currentNode -> getXlow(); 

    bool cutDir = currentNode->getCutDirection();
    if (xh <= maxWidth)
        cutDir = 1; // Flip cut direction if ratio is maxWidth


    if (currentNode -> getCutDirection() == 0) { // x high and x low are going to change for the children. Y high and y low are the same. 
        
        xPartition = round(((double)(localLeftArea*(xh - xl))/localTotalArea) + xl); 

        leftChild = new TreeNode( currentNode, xPartition, xl, yh, yl); // x and y values are assigned for these. 
        rightChild = new TreeNode(currentNode, xh, xPartition, yh, yl);  
    }
    else{
        // left is the same as down 
        yPartition = ceil(((double)localLeftArea * (yh - yl) / localTotalArea) + yl);

        leftChild = new TreeNode(currentNode, xh, xl, yPartition, yl); // x and y values are assigned for these. 
        rightChild = new TreeNode(currentNode, xh, xl, yh, yPartition); // left is the same as down and right is the same as up.  
    }

    currentNode -> setLeftChild(leftChild);
    currentNode -> setRightChild(rightChild);

    leftChild -> setParent(currentNode); 
    rightChild -> setParent(currentNode); 

    leftChild -> setCutDirection(cutDir); 
    rightChild -> setCutDirection(cutDir);

    bisection(&leftNodes, Nets, currentNode->getLeftChild());
    bisection(&rightNodes, Nets, currentNode->getRightChild());
}

void quadrature(vector<Node>* Nodes, vector<Net>* Nets, TreeNode* currentNode){  // root node should be made and sent to this function. 
    cout << "Node.size: " << Nodes->size() << endl;
    if(Nodes->size()== 1){// Only one node remaining so we're at the leaf. 
        currentNode -> setNodeId((*Nodes)[0].getID());
        (*Nodes)[0].setCoordinates(currentNode -> getXlow(), currentNode -> getYlow());
        currentNode->setLeftChild(nullptr);
        currentNode->setRightChild(nullptr);
        return; // you can't partition anymore. 
    }
    if (Nodes -> size() == 0) {return;}
    // Still need to add the xhigh, xlow, y high, ylow. These should be set when we set the root node and the we can use the area splits to get 
    // the new values for left and right children depending on the cutdirection of the current node. 
    // Also need to do the height and width thing but I think once we know the bounds for x and y then it should just be yhigh - y low and xhigh - xlow.
    // cout << "test1" << endl;
    int xh,xl,yh,yl;
    int xPartition, yPartition; // the x and y values that the cut goes through  
    // cout << "test1.2" << endl;
    int lastCut = FM(*Nodes, *Nets, Nodes->size()); // this is the cutsize.
    // cout << "test1.3" << endl;
    vector<Node> leftNodes; 
    vector<Node> rightNodes; 
    
    // cout << "test1.5" << endl;
    int localLeftArea = 0;
    int localRightArea = 0;
    int localTotalArea = 0;
    for (int i = 0; i < (*Nodes).size(); i++){
        if((*Nodes)[i].whichPartition() == 1){
            rightNodes.push_back((*Nodes)[i]); 
            localRightArea += (*Nodes)[i].getArea();
        }
        else {
            leftNodes.push_back((*Nodes)[i]);
            localLeftArea += (*Nodes)[i].getArea();
        }
        localTotalArea += (*Nodes)[i].getArea();
    }

    // cout << "Left size: " << leftNodes.size() << endl;
    // cout << "Right size: " << rightNodes.size() << endl;

    // cout << "test2" << endl;
    yh = currentNode -> getYhigh(); 
    yl = currentNode -> getYlow(); 
    // cout << "bruh" << endl;
    xh = currentNode -> getXhigh(); 
    xl = currentNode -> getXlow(); 
    // cout << "test2.2" << endl;

    TreeNode* leftChild;
    TreeNode* rightChild;

    // cout << "test2.3" << endl;
    // cout << "Cut DIR: " << currentNode -> getCutDirection() << endl;

    cout << "xh, xl, yh, yl: " << xh << " " << xl << " " << yh << " " << yl << endl;

    if (currentNode -> getCutDirection() == 0) { // x high and x low are going to change for the children. Y high and y low are the same. 
        // cout << "test2.4x" << endl;
        cout << "leftArea: " << localLeftArea << endl;
        cout << "totalArea: " << localTotalArea << endl;
        cout << "DA MATH: " << (((double)(localLeftArea*(xh - xl))/localTotalArea) + xl) << endl;
        cout << "TYPE OF DA MATH: "<<typeid(((double)(localLeftArea*(xh - xl))/localTotalArea) + xl).name() << endl;
        xPartition = round(((double)(localLeftArea*(xh - xl))/localTotalArea) + xl); 
        cout << "xPartition:" << xPartition << endl;
        // cout << "test2.5x" << endl;

        leftChild = new TreeNode( currentNode, xPartition, xl, yh, yl); // x and y values are assigned for these. 
        rightChild = new TreeNode(currentNode, xh, xPartition, yh, yl);  
    }
    else{
        // left is the same as down 
        // cout << "test2.4y" << endl;
        cout << "leftArea: " << localLeftArea << endl;
        cout << "totalArea: " << localTotalArea << endl;
        cout << "DA MATH: " << endl;
        cout << yh - yl << endl;
        cout << (double)localLeftArea * (yh - yl) << endl;
        cout << ((double)localLeftArea * (yh - yl)/localTotalArea) << endl;
        cout << (((double)localLeftArea * (yh - yl) / localTotalArea) + yl) << endl;
        cout << "TYPE OF DA MATH: "<<typeid(((double)localLeftArea * (yh - yl) / localTotalArea) + yl).name() << endl;
        yPartition = ceil(((double)localLeftArea * (yh - yl) / localTotalArea) + yl);
        cout << "yPartition:" << yPartition << endl;
        // cout << "test2.5y" << endl; 

        leftChild = new TreeNode(currentNode, xh, xl, yPartition, yl); // x and y values are assigned for these. 
        rightChild = new TreeNode(currentNode, xh, xl, yh, yPartition); // left is the same as down and right is the same as up.  
    }

    currentNode -> setLeftChild(leftChild);
    currentNode -> setRightChild(rightChild);

    leftChild -> setParent(currentNode); 
    rightChild -> setParent(currentNode); 

    bool flipDirection = !(currentNode -> getCutDirection()); 
    // cout << "test3" << endl;
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

void cutOrientedPlacement(vector<Node>* Nodes, vector<Net>* Nets, TreeNode* currentNode) {
    cout << "Node.size: " << Nodes->size() << endl;
    
    // Base case: only one node remaining (leaf node)
    if (Nodes->size() == 1) {
        currentNode->setNodeId((*Nodes)[0].getID());
        (*Nodes)[0].setCoordinates(currentNode->getXlow(), currentNode->getYlow());
        currentNode->setLeftChild(nullptr);
        currentNode->setRightChild(nullptr);
        return;
    }
    
    // If no nodes, return
    if (Nodes->size() == 0) {
        return;
    }
    
    // Retrieve current node boundaries
    int xh = currentNode->getXhigh();
    int xl = currentNode->getXlow();
    int yh = currentNode->getYhigh();
    int yl = currentNode->getYlow();
    
    cout << "xh, xl, yh, yl: " << xh << " " << xl << " " << yh << " " << yl << endl;
    
    // Determine best cut direction based on aspect ratio
    int width = xh - xl;
    int height = yh - yl;
    bool horizontalCut = (width > height);  // Cut horizontally if wider than tall
    
    // Run FM to get cutsize for both directions and choose the better one
    int cutSizeHorizontal = 0;
    int cutSizeVertical = 0;
    
    // Store original partition assignments
    vector<int> originalPartitions;
    for (int i = 0; i < Nodes->size(); i++) {
        originalPartitions.push_back((*Nodes)[i].whichPartition());
    }
    
    // Try horizontal cut first
    currentNode->setCutDirection(0);  // 0 = horizontal cut
    cutSizeHorizontal = FM(*Nodes, *Nets, Nodes->size());
    
    // Save horizontal partitioning
    vector<Node> horizontalLeftNodes;
    vector<Node> horizontalRightNodes;
    int horizontalLeftArea = 0;
    int horizontalRightArea = 0;
    
    for (int i = 0; i < Nodes->size(); i++) {
        if ((*Nodes)[i].whichPartition() == 1) {
            horizontalRightNodes.push_back((*Nodes)[i]);
            horizontalRightArea += (*Nodes)[i].getArea();
        } else {
            horizontalLeftNodes.push_back((*Nodes)[i]);
            horizontalLeftArea += (*Nodes)[i].getArea();
        }
    }
    
    // Reset partition assignments for vertical cut attempt
    for (int i = 0; i < Nodes->size(); i++) {
        (*Nodes)[i].setPartition(originalPartitions[i]);
    }
    
    // Try vertical cut
    currentNode->setCutDirection(1);  // 1 = vertical cut
    cutSizeVertical = FM(*Nodes, *Nets, Nodes->size());
    
    // Save vertical partitioning
    vector<Node> verticalLeftNodes;
    vector<Node> verticalRightNodes;
    int verticalLeftArea = 0;
    int verticalRightArea = 0;
    
    for (int i = 0; i < Nodes->size(); i++) {
        if ((*Nodes)[i].whichPartition() == 1) {
            verticalRightNodes.push_back((*Nodes)[i]);
            verticalRightArea += (*Nodes)[i].getArea();
        } else {
            verticalLeftNodes.push_back((*Nodes)[i]);
            verticalLeftArea += (*Nodes)[i].getArea();
        }
    }
    
    // Choose best cut direction based on cutsize
    vector<Node>* leftNodes;
    vector<Node>* rightNodes;
    int leftArea, rightArea, totalArea;
    int cutDirection;
    
    if (cutSizeHorizontal <= cutSizeVertical) {
        // Horizontal cut is better or equal
        cutDirection = 0;
        leftNodes = new vector<Node>(horizontalLeftNodes);
        rightNodes = new vector<Node>(horizontalRightNodes);
        leftArea = horizontalLeftArea;
        rightArea = horizontalRightArea;
    } else {
        // Vertical cut is better
        cutDirection = 1;
        leftNodes = new vector<Node>(verticalLeftNodes);
        rightNodes = new vector<Node>(verticalRightNodes);
        leftArea = verticalLeftArea;
        rightArea = verticalRightArea;
    }
    
    totalArea = leftArea + rightArea;
    currentNode->setCutDirection(cutDirection);
    
    // Calculate partition point based on chosen cut direction
    TreeNode* leftChild;
    TreeNode* rightChild;
    
    if (cutDirection == 0) {  // Horizontal cut
        int xPartition = round(((double)(leftArea * (xh - xl)) / totalArea) + xl);
        cout << "Horizontal cut at xPartition: " << xPartition << endl;
        
        leftChild = new TreeNode(currentNode, xPartition, xl, yh, yl);
        rightChild = new TreeNode(currentNode, xh, xPartition, yh, yl);
    } else {  // Vertical cut
        int yPartition = round(((double)(leftArea * (yh - yl)) / totalArea) + yl);
        cout << "Vertical cut at yPartition: " << yPartition << endl;
        
        leftChild = new TreeNode(currentNode, xh, xl, yPartition, yl);
        rightChild = new TreeNode(currentNode, xh, xl, yh, yPartition);
    }
    
    // Set up tree connections
    currentNode->setLeftChild(leftChild);
    currentNode->setRightChild(rightChild);
    leftChild->setParent(currentNode);
    rightChild->setParent(currentNode);
    
    // Recursively place nodes in children
    cutOrientedPlacement(leftNodes, Nets, leftChild);
    cutOrientedPlacement(rightNodes, Nets, rightChild);
    
    // Clean up allocated vectors
    delete leftNodes;
    delete rightNodes;
}