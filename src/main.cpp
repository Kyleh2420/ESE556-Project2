#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
#include <limits.h>
#include <cstring>
#include <tuple>

#include "Node.h"
#include "Net.h"
#include "LinkedList.h"
#include "breuers.cpp"
#include "fiducciaMattheyses.cpp"
#include "fileop.cpp"
#include "shared_variables.h"

using namespace std;

int main(int argc, char *argv[]) {

    /*
        This will process all the command line args for this program to run.
    */
    string benchmark;
    int logLevel;
    // Parse command-line arguments
    parseArguments(argc, argv, benchmark, logLevel);
    logLevelGlobal = logLevel;

    // string benchmark = "superblue18";
    // cout << "Which benchmark would you like to run? (Blank will run superblue18)" << endl;
    // cin >> benchmark;

    string ifilepath = "../Benchmarks/" + benchmark + "/" + benchmark;
    string ofilepath = "../Output/" + benchmark + "_Output.txt";

    //Parse the nodes
    vector<Node> Nodes;
    Nodes = parseNodes(ifilepath+".nodes");
    if (logLevel > 0) {
        cout << "Number of nodes: " << Nodes.size() << endl;
    }

    //Parse the nets
    vector<Net> Nets;
    Nets = parseNets(ifilepath+".nets", &Nodes);
    if (logLevel > 0) {
        cout << "Number of nets: " << Nets.size() << endl;
    }

    //Parse the SCL file
    parseSCL(ifilepath+".scl");
    if (logLevel > 0) {
        cout << "Number of rows: " << numRows << endl;
        cout << "Number of sites: " << numSites << endl;
    }

    //Parse the PL file
    parsePL(ifilepath+".pl", &Nodes);
    if (logLevel > 0)
        cout << "xcoord: " << get<0>(Nodes[offset].getCoordinates()) << " ycoord: " << get<1>(Nodes[offset].getCoordinates()) << endl;
    
    

    // if (logLevel > 1) {
    //     cout << "Node 45 ID: " << Nodes[45].getID() << endl;
    //     vector<int> test = Nodes[45].getConnectedNets();
    //     for (auto i : test) {
    //         cout << i << " ";
    //     }
    //     cout << endl;
    // }

    //Check the area constructor
    // for (int i = 146; i < 166; i++) {
    //     cout << Nodes[i].getArea() << endl;
    // }

    //At this time, we should have everything parsed into the correct data structures

    

    //Create the inital cut
    //Randomly assign all nodes a status of either left or right
    srand(time(0));
    
    //Execute FM
    int lastCut = FM(&Nodes, &Nets);
    

    writeOutput(ofilepath,lastCut,Nodes);
    return 0;   
}


