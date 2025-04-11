#ifndef Node_h
#define Node_h

#include <string>
#include <vector>
#include <tuple>
using namespace std;

class Node
{
private:
    string ID_;
    int width_;
    int height_;
    int area; // area of the cell.  
    bool terminal_;
    vector<int> connectedNets; // a vector of index of nets connected to this given node as they are all sequentially given in the .nets file.
    int xcoord; //  lower left x coordinate of the node
    int ycoord; //  lower left y coordinate of the node
    
    bool locked;        //0 for unlocked, 1 for locked
    bool partition;     //This is going to be left or right (0 or left, 1 for right)
    int crossings;      //This is used to determine the gain sizes

public:

    Node(string ID, int width, int height, bool terminal = false) {
        ID_ = ID;
        width_ = width;
        height_ = height;
        terminal_ = terminal;
        area = width * height;
        crossings = 0;
        locked = 0;
    };
    ~Node() {};

    string getID() { return ID_; }
    int getWidth() { return width_; }
    int getHeight() { return height_; }
    int getArea() { return area; }
    bool isTerminal() { return terminal_; }
    void addNet(int i) { connectedNets.push_back(i); }
    
    vector<int>& getConnectedNets() { return connectedNets; }

    //Related to partitions
    void movePartition() {partition = !partition;} // changes the partition of the node.
    bool whichPartition() {return partition;} // returns 1 if partition 1. 
    void setPartition(bool side) {partition = side;}    //Sets the partition to whatever the input is

    //Used to calculate gains
    void incCrossings() {crossings++;}
    void decCrossings() {crossings--;}
    int getCrossings() {return crossings;}
    void setCrossings(int i) {crossings = i;}

    //Lock and unlock
    void lockNode() {locked = 1;}
    void unlockNode() {locked = 0;}
    bool getLockStatus() {return locked;}

    //Used to get the lower left coordinates of the node
    tuple<int, int> getCoordinates() {return make_tuple(xcoord, ycoord);}
    void setCoordinates(int x, int y) {xcoord = x; ycoord = y;}
};



#endif //Node_H