#ifndef Net_h
#define Net_h

#include <vector>
#include <string>
#include "Node.h"
#include "LinkedList.h"

class Net
{
private:
    vector<int> connectedNodes_; // This vector contains the index for the nodes connected to the net
    string netName_;
    std::pair<int, int> distribution; // <count_in_partition_0, count_in_partition_1>
public:
    Net(string netName, vector<int> net){
        netName_ = netName;
        connectedNodes_ = net;
    };
    ~Net(){};
    string getName() {return netName_;}
    vector<int>& getConnectedNodes() { return connectedNodes_; }

    // Calculates and updates the distribution of its cells based on the current state of Nodes
    void updateDistribution(std::vector<Node>& all_nodes) {
        distribution.first = 0;
        distribution.second = 0;
        cout << "Net " << netName_ << " distribution: ";
        cout << "count in partition 0: " << distribution.first << " count in partition 1: " << distribution.second << endl;
        for (int node_idx : connectedNodes_) {
            // cout << " updating " <<all_nodes[node_idx].getID() << endl;
            if (all_nodes[node_idx].isTerminal()) continue; // Or handle terminals based on specific rules
            if (all_nodes[node_idx].whichPartition() == 0) {
                distribution.first++;
            } else {
                distribution.second++;
            }
        }
    }

    std::pair<int, int> getDistribution() const { return distribution; }
};

#endif