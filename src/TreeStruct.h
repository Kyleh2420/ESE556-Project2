#ifndef TreeStruct_h
#define TreeStruct_h

using namespace std; 

class TreeStruct
{
private:
    /* data */
    TreeStruct* rightChild_; 
    TreeStruct* leftChild_; 
    int width_; 
    int height_; 
    int NodeIndex; // This is the index of the node in the vector nodes. Everything else is gonna be -1 unless  we get to the leaf node. 

public:
    TreeStruct(){
        rightChild_ = nullptr; 
        leftChild_ = nullptr; 
        NodeIndex = -1; 
        width_ = -1; 
        height_ = -1; 
    };
    ~TreeStruct();

    int getWidth() {return width_;}
    int getHeight() {return height_;}
    TreeStruct* getRightChild() {return rightChild_;}
    TreeStruct* getLeftChild() {return leftChild_;}

    void setRightChild(TreeStruct* right)  {rightChild_ = right; }
    void setLeftChild(TreeStruct* left)  {leftChild_ = left;}    
    void setNodeIndex(int idx) {NodeIndex = idx;}
    void setWidth(int width){width_ = width;}
    void setHeight(int height){height_ = height;}
        
};


#endif 
