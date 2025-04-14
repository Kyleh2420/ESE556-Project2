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

public:
    TreeNode(){
        rightChild_ = nullptr; 
        leftChild_ = nullptr; 
        NodeIndex = -1; 
        width_ = -1; 
        height_ = -1; 
    };
    ~TreeNode();

    int getWidth() {return width_;}
    int getHeight() {return height_;}
    TreeNode* getRightChild() {return rightChild_;}
    TreeNode* getLeftChild() {return leftChild_;}

    void setRightChild(TreeNode* right)  {rightChild_ = right; }
    void setLeftChild(TreeNode* left)  {leftChild_ = left;}    
    void setNodeIndex(int idx) {NodeIndex = idx;}
    void setWidth(int width){width_ = width;}
    void setHeight(int height){height_ = height;}
        
};


#endif 
