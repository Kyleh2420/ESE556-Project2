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

#include "shared_variables.h"
#include "Node.h"
#include "Net.h"
#include "LinkedList.h"
#include "breuers.cpp"
// #include "fiducciaMattheyses.cpp"
#include "fm.cpp"
#include "fileop.cpp"
#include "TreeNode.h"
#include "HPWL.cpp"

using namespace std;

int main(int argc, char *argv[]) {

    /*
        This will process all the command line args for this program to run.
    */
    string benchmark;
    int logLevel;
    int placementAlgo = 0;
    // Parse command-line arguments
    parseArguments(argc, argv, benchmark, logLevel, placementAlgo);
    logLevelGlobal = logLevel;
    cout << placementAlgo << endl;

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
    // parsePL(ifilepath+".pl", &Nodes);
    // if (logLevel > 0)
    //     cout << "xcoord: " << get<0>(Nodes[offset].getCoordinates()) << " ycoord: " << get<1>(Nodes[offset].getCoordinates()) << endl;
    
    

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

    // //Randomly assign partitions
    // for (int i = 0; i < Nodes.size(); i++) {
    //     int random = rand() % 2;
    //     Nodes[i].setPartition(random);
    // }

    int numNodes = Nodes.size(); // Set this appropriately


    float balance_low = 0.5f; 
    float balance_high = 0.5f;
    TreeNode *rootNode = new TreeNode(nullptr, numSites, 0, numRows, 0); // Create the root node
    if (placementAlgo == 1)
        quadrature(&Nodes, &Nets, rootNode); // Call quadrature with nullptr as the parent for the rootNode
    else if (placementAlgo == 2)
        bisection(&Nodes, &Nets, rootNode); // Call bisection with nullptr as the parent for the rootNode
    else if (placementAlgo == 3)
        slice(&Nodes, &Nets, rootNode, numSites); // Call slice with nullptr as the parent for the rootNode
    else if (placementAlgo == 4)
        cout << "I aint doing this shitty fucking placement algo. Shit makes no sense. BUT I LOVE ALEX DOBOLI. HIS SON GOES TO STANFORD." << endl;
    else {
        cerr << "Invalid input. Please enter '\n\t1: Quadrature\n\t2: Bisection\n\t3: Slice Bisection\n\t4: Cut Oriented" << endl;
        return 1;
    }

    clearOutputFile(ofilepath);
    writeOutputPreOrder(ofilepath,rootNode);

    parseCoordinates(ofilepath,Nodes);
    ofstream out;
    out.open(ofilepath,std::ios::app);
    out << "Total Wirelength: "<<totalWirelength(Nodes, Nets) << endl;

    out.close();

    return 0;   
}


