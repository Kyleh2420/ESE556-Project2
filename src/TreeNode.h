#ifndef TreeNode_h
#define TreeNode_h

using namespace std; 

class TreeNode
{
private:
    /* data */
    TreeNode* rightChild_; 
    TreeNode* leftChild_; 
    int width_; 
    int height_; 
    int NodeIndex; // This is the index of the node in the vector nodes. Everything else is gonna be -1 unless  we get to the leaf node. 
    int xhigh; 
    int xlow; 
    int yhigh; 
    int ylow;
    int yhigh; 
    bool cutDirection;

public:
    TreeNode(){
        rightChild_ = nullptr; 
        leftChild_ = nullptr; 
        NodeIndex = -1; 
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
    int getLeafNodeIndex(){ return NodeIndex;}
    int getXlow(){return xlow;}
    int getXhigh(){return xhigh;}
    int getYlow(){return ylow;}
    int getYhigh(){return yhigh;}
    bool getCutDirection(){return cutDirection;}

    void setRightChild(TreeNode* right)  {rightChild_ = right; }
    void setLeftChild(TreeNode* left)  {leftChild_ = left;}    
    void setNodeIndex(int idx) {NodeIndex = idx;}
    void setWidth(int width){width_ = width;}
    void setHeight(int height){height_ = height;}
    void setxhigh(int xh){xhigh = xh;}
    void setxlow(int xl){xlow = xl;}
    void setyhigh(int yh){yhigh = yh;}
    void setxhigh(int yl){ylow = yl;}
    void setCutDirection(bool dir){cutDirection = dir;}     
};


#endif 
