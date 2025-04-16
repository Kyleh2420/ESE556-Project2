#ifndef TreeNode_h
#define TreeNode_h

using namespace std; 

class TreeNode
{
private:
    /* data */
    TreeNode* rightChild_; 
    TreeNode* leftChild_; 
    TreeNode* Parent_; 
    int width_; 
    int height_; 
    string NodeId; // This is the ID of the node that is parsed and read from the flie. Everything else is gonna be -1 unless  we get to the leaf node. 
    int xhigh; 
    int xlow; 
    int yhigh;
    int ylow; 
    bool cutDirection;

public:
    TreeNode(){
        rightChild_ = nullptr; 
        leftChild_ = nullptr; 
        Parent_ = nullptr; 
        NodeId = ""; 
        width_ = -1; 
        height_ = -1;
        xhigh = -1; 
        xlow = -1; 
        yhigh = -1; 
        ylow = -1; 
        cutDirection = 0; 

    };
    ~TreeNode();

    int getWidth() {return width_;}
    int getHeight() {return height_;}
    TreeNode* getRightChild() {return rightChild_;}
    TreeNode* getLeftChild() {return leftChild_;}
    TreeNode* getParent(){return Parent_;}
    string getLeafNodeId(){ return NodeId;} // this is the node id of the node that is going to be stored in the leaf node of the tree. So the string from class node. 
    int getXlow(){return xlow;}
    int getXhigh(){return xhigh;}
    int getYlow(){return ylow;}
    int getYhigh(){return yhigh;}
    bool getCutDirection(){return cutDirection;}

    void setRightChild(TreeNode* right)  {rightChild_ = right; }
    void setLeftChild(TreeNode* left)  {leftChild_ = left;}  
    void setParent(TreeNode* Par) {Parent_ = Par;} 
    void setNodeId(string idx) {NodeId = idx;}
    void setWidth(int width){width_ = width;}
    void setHeight(int height){height_ = height;}
    void setxhigh(int xh){xhigh = xh;}
    void setxlow(int xl){xlow = xl;}
    void setyhigh(int yh){yhigh = yh;}
    void setylow(int yl){ylow = yl;}
    void setCutDirection(bool dir){cutDirection = dir;}     
};


#endif 
