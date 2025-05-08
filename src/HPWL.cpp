#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "Node.h"
#include "Net.h"

using namespace std;

void parseCoordinates(string filepath, vector<Node>& nodes) {
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filepath << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip empty lines and comments
        stringstream ss(line);
        string nodeName,temp,x,y;
        int xcoord, ycoord;
        ss >> temp >> temp >> nodeName >> temp >> x >> y;
        xcoord = stoi(x.substr(1, x.size()-1));
        ycoord = stoi(y.substr(0, y.size()-1));
        //cout << nodeName << " " << xcoord << " " << ycoord << endl;
        int nodeIndex = stoi(nodeName.substr(1));
        nodes[nodeIndex].setCoordinates(xcoord, ycoord);
    }
    
}

int totalWirelength(vector<Node>& nodes, vector<Net>& nets) {
    int totalWirelength = 0;

    for (Net net : nets) {
        for (int nodeIndex : net.getConnectedNodes()) {
            if (nodeIndex > nodes.size()-1) continue; // Basic bounds check
            totalWirelength += abs(nodes[nodeIndex].getX() - nodes[nodeIndex+1].getX() )+abs(nodes[nodeIndex].getY() - nodes[nodeIndex+1].getY());
        }
    }
    return totalWirelength;
}