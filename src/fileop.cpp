#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
#include <limits.h>
#include <cstring>

#include "Node.h"
#include "Net.h"
#include "LinkedList.h"
#include "shared_variables.h"

using namespace std;

// Global variables
int numNodes = -1;
int numTerm = -1;
int numNets = -1;
int offset = 0;
int logLevelGlobal = 0;
int numRows = -1;
int numSites = -1;

// Function to parse command-line arguments
void parseArguments(int argc, char *argv[], string &benchmark, int &logLevel) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " -log X -input Test" << endl;
        // cerr << "Usage: " << argv[0] << " -log X -input Test" << endl;
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-log") == 0) {
            if (i + 1 < argc) {
                logLevel = atoi(argv[i + 1]);
                i++;
            } else {
                cerr << "Error: -log option requires an argument." << endl;
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "-input") == 0) {
            if (i + 1 < argc) {
                benchmark = argv[i + 1];
                i++;
            } else {
                cerr << "Error: -input option requires an argument." << endl;
                exit(EXIT_FAILURE);
            }
        } else {
            cerr << "Unknown option: " << argv[i] << endl;
            exit(EXIT_FAILURE);
        }
    }
}

vector<Node> parseNodes(string filename) {
    ifstream NodeFile(filename);
    if(!NodeFile.is_open()) {
        cerr << filename << " not found" << endl;
        exit(EXIT_FAILURE);
    }

    vector<Node> Nodes;
    char const* digits = "0123456789";
    string line;
    bool flag = 0;

    getline(NodeFile,line); //gets first line

    //gets Number of Nodes
    while (numNodes == -1) {
        getline(NodeFile,line);
        if (line.empty() || line[0] == '#') continue;
        numNodes = stoi(line.substr(line.find_first_of(digits)));
    }
   
    getline(NodeFile,line); //gets NumTerminals line (not needed)
    numTerm = stoi(line.substr(line.find_first_of(digits)));

    //Puts nodes into vector
    while (Nodes.size()<numNodes||!NodeFile.eof()) {
        getline(NodeFile,line);
        if (line.empty() || line[0] == '#') continue;
        stringstream iss(line);
        string nodeID;
        int width;
        int height;
        bool terminal = false;
        iss >> nodeID >> width >> height;
        if(nodeID.find("p") != string::npos&&flag == 0) {
            flag = 1;
            offset = Nodes.size();
        }
        if(line.find("terminal") != string::npos)
            terminal = true;
        Nodes.push_back(Node(nodeID,width,height,terminal));
    }

    NodeFile.close();
    return Nodes;
}

vector<Net> parseNets(string filename, vector<Node>* nodes) {
    ifstream NetFile(filename);
    if(!NetFile.is_open()) {
        cerr << filename << " not found" << endl;
        exit(EXIT_FAILURE);
    }

    vector<Net> Nets;
    char const* digits = "0123456789";
    string line;

    getline(NetFile,line); //gets first line

    //gets Number of Nets
    while (numNets == -1) {
        getline(NetFile,line);
        if (line.empty() || line[0] == '#') continue;
        numNets = stoi(line.substr(line.find_first_of(digits)));
    }

    getline(NetFile,line); //gets N(line.substr(liumPins line (not needed)

    while (Nets.size()<numNets||!NetFile.eof()) {
        getline(NetFile,line);
        if (line.empty() || line[0] == '#') continue;
        stringstream iss(line);
        string t;
        int numNodes;
        string netName;
        vector<int> indicies;
        iss >> t >> t >> numNodes >> netName;
        int netIndex = stoi(netName.substr(1));
        //cout << numNodes << " " << netName << endl;
        for (int i = 0; i < numNodes; i++) {
            getline(NetFile,line);
            if (line.empty() || line[0] == '#') continue;
            stringstream iss(line);
            string nodeName;
            iss >> nodeName;
            int nodeIndex = stoi(nodeName.substr(1));
            if (nodeName.find("p") != string::npos)
            {
                nodeIndex += offset;
                indicies.push_back(nodeIndex);
            }
            else
                indicies.push_back(nodeIndex);
            
            (*nodes)[nodeIndex].addNet(netIndex);
        }
        Net net(netName, indicies);
        Nets.push_back(net);
    }

    NetFile.close();
    return Nets;
}

void parseSCL(string filename) {
    ifstream SCLFile(filename);
    if(!SCLFile.is_open()) {
        cerr << filename << " not found" << endl;
        exit(EXIT_FAILURE);
    }

    char const* digits = "0123456789";
    string line;
    bool flag = 0;

    getline(SCLFile,line); //gets first line

    while (numRows == -1) {
        getline(SCLFile,line);
        if (line.empty() || line[0] == '#') continue;
        numRows = stoi(line.substr(line.find_first_of(digits)));
    }

    while (numSites == -1) {
        getline(SCLFile,line);
        if (line.empty() || line[0] == '#') continue;
        if (line.find("NumSites") != string::npos) {
            // cout << "NumSites found" << endl;
            // cout << line << endl;
            // cout << line.substr(line.find("NumSites"))<< endl;
            line = line.substr(line.find("NumSites"));
            numSites = stoi(line.substr(line.find_first_of(digits)));
        }
    }
    
    SCLFile.close();
}

void parsePL(string filename, vector<Node>* nodes) {
    ifstream PLFile(filename);
    if(!PLFile.is_open()) {
        cerr << filename << " not found" << endl;
        exit(EXIT_FAILURE);
    }

    char const* digits = "0123456789";
    string line;

    getline(PLFile,line); //gets first line

    while (getline(PLFile,line)) {
        if (line.empty() || line[0] == '#') continue;
        stringstream iss(line);
        string nodeName;
        int xcoord, ycoord;
        iss >> nodeName >> xcoord >> ycoord;
        int nodeIndex = stoi(nodeName.substr(1));
        if (nodeName.find("p") != string::npos)
            nodeIndex += offset;
        (*nodes)[nodeIndex].setCoordinates(xcoord, ycoord);
    }

    PLFile.close();
}

void writeOutput(string filename, int cutsize, vector<Node> Nodes) {
    ofstream OutputFile(filename);
    if(!OutputFile.is_open()) {
        cerr << filename << " not found" << endl;
        exit(EXIT_FAILURE);
    }
    // making a copy and not deleting this just yet. 
    vector<string> leftNodes;
    vector<string> rightNodes;
    int leftArea = 0;
    int rightArea = 0;

    OutputFile << "Cutsize: " << cutsize << endl;
    for (int i = 0; i < numNodes; i++) {
        if (Nodes[i].whichPartition() == 0) {
            leftNodes.push_back(Nodes[i].getID());
            leftArea += Nodes[i].getArea();
        } else {
            rightNodes.push_back(Nodes[i].getID());
            rightArea += Nodes[i].getArea();
        }
    }
    float ratio = (float)leftArea/rightArea;
    OutputFile << "Partition Ratio: " << ratio << endl;

    int totalArea = leftArea + rightArea;
    float leftPercent = (float)leftArea/totalArea * 100;
    float rightPercent = (float)rightArea/totalArea * 100;

    OutputFile << "Partition 1: " << leftNodes.size() << "\tArea: " << leftArea << " - " << leftPercent << "%" << endl;
    for (auto i : leftNodes) {
        OutputFile << i << endl;
    }

    OutputFile << "Partition 2: " << rightNodes.size() << "\tArea: " << rightArea << " - " << rightPercent << "%" << endl;
    for (auto i : rightNodes) {
        OutputFile << i << endl;
    }

    OutputFile.close();
}