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
    TreeNode(TreeNode *root, int x_high, int x_low, int y_high, int y_low){
        rightChild_ = nullptr; 
        leftChild_ = nullptr; 
        Parent_ = root; 
        NodeId = ""; 
        width_ = -1; 
        height_ = -1;
        xhigh = x_high; 
        xlow = x_low; 
        yhigh = y_high; 
        ylow = y_low; 
        cutDirection = 0; // if cut direction is 0 then vertical(x high x low change) if 1 then horizontal cut(y high y low change). 

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
