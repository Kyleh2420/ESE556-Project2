#include <iostream> 
#include <fstream> // writing the nodes and the nets to a file. 
#include <cstdlib>
#include <vector>
using namespace std; 


void generateNodes(int nodes, string filename, int numPins, int npt){
    //int numPins; // number of p nodes 
    int terminals; // numer of terminals.
    //int npt ; //  non pin terminals. 

    terminals = numPins +npt; 
    std :: ofstream nodesFile(filename); // open the file 
    if (!nodesFile.is_open()) {
        cout << "Can't open file" << endl; 
        exit(1); 
    }
    else {
        nodesFile << "SBU ESE 566 WKK" << endl; 
        nodesFile << "NumNodes : " << nodes << endl; 
        nodesFile << "NumTerminals : "<< terminals << endl; 

        for(int i = 0; i < nodes-(numPins+npt); i++){ // nodes - numPins are 0 nodes. 
            //cout <<i<<endl; 
            nodesFile <<"o"<<i<<"\t"<< rand()%11 + 1 <<"\t" << rand()%11 + 1<< endl; // o pins, the pins that can either be movable or non movable and start with o  
        }
        for (int i =nodes-(numPins+npt); i < nodes- numPins ; i++){
            nodesFile <<"o"<<i<<"\t"<< rand()%11 + 1 <<"\t" << rand()%11 + 1<<"\t"<< "terminal"<< endl;
        }

        for (int i=0; i < numPins; i++){
            nodesFile <<"p"<<i<<"\t"<< rand()%11 + 1 <<"\t" << rand()%11 +1 << "\t" << "terminal_NI" << endl; // p pins. 
        }
    }

    nodesFile.close();
}

void generateNets (int nodes, int nets, int pins, int npt, string filename){
    std :: ofstream netFile(filename);
    vector<int> store_nodes; 
    int random; 
    int degree; // degree of the net. 
    int nodeForNet; // this is for us to get a node. 
    if(!netFile.is_open()){
        cout<< "Can't open the nets file" << endl; 
        exit(1); 
    }
    else {
        netFile << "SBU ESE 566 WKK" << endl; 
        netFile << "NumNets : " << nets << endl; 
        netFile << "NumPins : "<< pins << endl; 

        for (int i = 0; i < nets ; i++){
            store_nodes.clear(); 
            degree = (rand()%nodes) + 2; // a net can be connected to all the nodes but never to zero nodes. 
            if (degree > nodes){degree = nodes;} // a net can't have a degree of more than the number of nodes. 
            netFile << "NetDegree\t:\t" << "\t" << degree << "\t" << "n" << i << endl; 
            for (int j =0 ; j<degree; j++){
                random = rand() % nodes;// cuz we can choose from a selection of 0 to nodes-1 nodes.  
                bool exists = false; 
                if (store_nodes.size()!= 0){
                    for(int k = 0; k < store_nodes.size(); k++){
                        if(random == store_nodes[k]){
                            j--; // because we found a value for this iteration that is useless and the value of j has been incremented already, decremenet the value and redo the iteration. 
                            exists = true; 
                            break; 
                            //cout <<"hi"<<endl;
                        }  
                    }
                }    
                if (!exists || store_nodes.size()==0) { 
                    if (random < nodes -pins){ // this would make it a o non terminal node. 
                        netFile << "\t"<< "o"<<random<<endl; 
                        //cout << "\t"<< "o"<<random<<endl; 
                    }
                    else{
                        netFile << "\t"<<"p"<<random - (nodes - pins)<< endl; 
                        //cout << "\t"<< "p"<<random<<endl; 
                    }
                    store_nodes.push_back(random);
                    // break; 
                }           
            }
        
        }
    }
    netFile.close();
}


int main (){
    string benchmark = "Test";
    string filePath = "../Benchmarks/" + benchmark + "/" + benchmark; 
    int numNodes; 
    int numNets; // user intputs for num nodes and num nets. 

     
     

    cout <<"Enter the number of nodes" << endl;   
    cin >> numNodes; 
    cout << "Enter the number of nets in the graph" << endl;
    cin  >> numNets; 
    int Pins = (rand() % ((int)(numNodes/3) + 1)) + 1; // at most a third of the nodes can be pins. 
    int npt = (rand()% (int)((numNodes-Pins)/5)) + 1;// the number of o nodes that are terminals. 
    //cout <<"pins" << Pins << endl; 
    //cout <<"npt" << npt << endl; 
    generateNodes(numNodes, filePath+".nodes", Pins, npt); 
    generateNets(numNodes, numNets, Pins, npt, filePath+".nets"); 
}