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

using namespace std;

int numNodes = -1;
int numTerm = -1;
int numNets = -1;
int offset;
int logLevelGlobal;

struct timePoint { 
    int lockedNode; 
    int cutSize; 
    float ratio;
};



float getAreaRatio(vector<Node> Nodes){
    int leftArea, rightArea; 
    for (int i = 0; i < numNodes; i++) {
        if (Nodes[i].whichPartition() == 0) {
            //leftNodes.push_back(Nodes[i].getID());
            leftArea += Nodes[i].getArea();
        } else {
            //rightNodes.push_back(Nodes[i].getID());
            rightArea += Nodes[i].getArea();
        }
    }
    float ratio = (float)leftArea/rightArea;
    return ratio; 
}

int calculateCrossings(vector<Node>* Nodes, vector<Net>* Nets) {
    int cutsize = 0;
    bool flag = false;  // Used to see if we should break loop and increment the crossings for the nets

    //This is going to be O(n^3) because I'm spending too much brainpower trying to figure out how to minimize it
    //Our goal is to find the number of net crossings that occur for each node
    //We already know the total number of nets for each node
    //Noncrossings = totalNets - crossings
    //Gain = crossings - noncrossings
    //Therefore: gain = 2*crossings - totalNets
    
    //We will spend O(n^3)
    //Loop through all nets
        //Loop through each node on the net
            //Compare this to every other node (besides itself) and count the number of crossings
        //Store that number into node.crossings
    for (size_t i = 0; i < Nets->size(); i++) {
        for (size_t j = 0; j < (*Nets)[i].getConnectedNodes().size(); j++) {
            int crossings = 0;
            int nodejIdx = (*Nets)[i].getConnectedNodes()[j];
            for (size_t k = 0; k < (*Nets)[i].getConnectedNodes().size(); k++) {
                int nodekIdx = (*Nets)[i].getConnectedNodes()[k];
                if (j == k) break;
                if ((*Nodes)[nodejIdx].whichPartition() ^ (*Nodes)[nodekIdx].whichPartition()) {
                    //In here, there is a crossing.
                    crossings++;
                    flag = true;
                }
            }
            (*Nodes)[nodejIdx].setCrossings(crossings);
        }
        //We consider cutsize on a per net basis, so it goes here
        if (flag) {
            cutsize++;
        }
    }
    return cutsize;
}

void storeInBuckets(map<int, linkedlist*>* leftBucket, map<int, linkedlist*>* rightBucket, vector<Node>* Nodes, int* lSize, int* rSize) {
    *lSize = 0;
    *rSize = 0;
    
    map<int, linkedlist*> *currentBucket; //Used to effectively swap between the two buckets
    //Seperate the nodes out to the buckets, dependant on parition and gain.
    for (int i = 0; i < numNodes; i++) {
        if (logLevelGlobal > 2) {
            cout << (*Nodes)[i].getID() << " placed in ";
        }
        
        
        if ((*Nodes)[i].whichPartition() == 1) {
            //If the partition is the left
            currentBucket = leftBucket;
            (*lSize)++;
            if (logLevelGlobal > 2) {
                cout << "Left  ";
            }
        } else {
            //If the partition is on the right
            currentBucket = rightBucket;
            (*rSize)++;
            if (logLevelGlobal > 2) {
                cout << "Right ";
            }
        }

        //Calculate the gain (See function calculateCrossings for more information)
        //gain = 2*crossings - totalNets
        int gain = 2*(*Nodes)[i].getCrossings() - (*Nodes)[i].getConnectedNets().size();
        if (logLevelGlobal > 2) {
            cout << "Gain of " << gain << " = (2 *" << (*Nodes)[i].getCrossings() << ") - " << (*Nodes)[i].getConnectedNets().size();
        }
        

        //Create a new DLLnode, whose value is the index of the node
        // Allocate memory on the heap so it persists after the loop iteration
        linkedlist* insertDLLNode = new linkedlist(i);


        //currentBucket is now set to either the left or right bucket, depending on which parition the current node is
        if ((*currentBucket).find(gain) == (*currentBucket).end()) {
            //If the key is not present

            //Insert this DLL node's address to the hashmap with key of crossings
            (*currentBucket)[gain] = insertDLLNode;
            if (logLevelGlobal > 2) {
                cout << " No key" << endl;
            }
        } else {
            //If the key is present
            if (logLevelGlobal > 2) {
                cout << " w/ key" << endl;
            }

            //Get the node at that DLL and follow it until the very end.
            linkedlist* dllNode = (*currentBucket)[gain];
            if (dllNode != nullptr) {
                while (dllNode->getNext() != nullptr) {
                    dllNode = dllNode->getNext();
                }
                //We should be at the last node
                //Set this node to the next DLLNode and attach the pointer references
                dllNode->setNext(insertDLLNode);
                insertDLLNode->setPrev(dllNode);
            }
        }
    }
}

void removeFromBucket(map<int, linkedlist*>* chosenBucket, int gainSelector, int selectedNode, vector<Node>* Nodes) {
    // Loop to the end of the linked list
    linkedlist* nodeUpdater = (*chosenBucket)[gainSelector];
    if (logLevelGlobal > 2) {
        cout << "\t\tEntered removing from bucket" << endl;
    }
    if (nodeUpdater == nullptr) {
        cerr << "Error: nodeUpdater is nullptr" << endl;
        exit(EXIT_FAILURE);
    }
    if (logLevelGlobal > 2) {
        cout << "\t\tLooping until the end looking for " << selectedNode << " in gain " << gainSelector << endl;
    }
    // Loop until the end of the node
    // Or until we find the index we're looking for
    while (nodeUpdater->getNodeID() != selectedNode) {
        if (logLevelGlobal > 2) {
            cout <<"\t\tCurrently at node[" << nodeUpdater->getNodeID() << "]: " << (*Nodes)[nodeUpdater->getNodeID()].getID() << endl;
        }
        if (nodeUpdater->getNext() == nullptr) {
            //If this triggers, we are at the end of the list
            cerr << "Error: nodeUpdater->getNext() is nullptr" << endl;
            exit(EXIT_FAILURE);
        }
        nodeUpdater = nodeUpdater->getNext();
    }
    if (logLevelGlobal > 2) {
        cout << "\t\tFound the node to delete" << endl;
    }
    
    // nodeUpdater now has the node we're trying to move
    // Update the DLL to remove nodeUpdater from the area
    // If nodeUpdater.getPrev() is nullptr, it is the first.
    // If nodeUpdater.getNext() is nullptr, it is the last.
    // If it is both, remove the key entirely from the map
    linkedlist* nodeUpdaterPrev = nodeUpdater->getPrev();
    linkedlist* nodeUpdaterNext = nodeUpdater->getNext();
    if (logLevelGlobal > 2) {
        cout << "\t\tGrabbed next and prev" << endl;
    }
    // Set nodeupdater to default nullptrs for both
    nodeUpdater->setPrev(nullptr);
    nodeUpdater->setNext(nullptr);
    if (logLevelGlobal > 2) {
	    cout << "\t\tSet nodeUpdater to null" << endl;
    }
    if (nodeUpdaterPrev == nullptr && nodeUpdaterNext == nullptr) {
        if (logLevelGlobal > 2) {
	        cout << "\t\tRemoving key from bucket" << endl;
        }
        (*chosenBucket).erase(gainSelector);
    } else if (nodeUpdaterPrev == nullptr && nodeUpdaterNext != nullptr) {
        if (logLevelGlobal > 2) {
	        cout << "\t\tGet the next node and move it to the head" << endl;
        }
        // This is the first node, link this to the head in the key
        (*chosenBucket)[gainSelector] = nodeUpdaterNext;
        nodeUpdaterNext->setPrev(nullptr);
    } else if (nodeUpdaterPrev != nullptr && nodeUpdaterNext == nullptr) {
        // This is the last node, simply remove the previous connection
        if (logLevelGlobal > 2) {
	        cout << "\t\tThis is the last node, set prev to null" << endl;
        }
        nodeUpdaterPrev->setNext(nullptr);
    } else {
        // Standard case, simply stitch the DLL back together
        if (logLevelGlobal > 2) {
	        cout << "\t\tFound in the middle" << endl;
        }
        nodeUpdaterPrev->setNext(nodeUpdaterNext);
        nodeUpdaterNext->setPrev(nodeUpdaterPrev);
        
    }
}

void printBuckets(map<int, linkedlist*> leftBucket, map<int, linkedlist*> rightBucket, int selectedNode, vector<Node>* Nodes, vector<timePoint> timeline) {
    if (selectedNode != -1) {
        cout << "move and lock node " << (*Nodes)[selectedNode].getID() << endl;
    }
    else 
        cout << "No node selected" << endl;
    
    cout << "Left Bucket" << endl;
    for (auto i : leftBucket) {
        cout << "\tGain: " << i.first << "\t\t";
        linkedlist* dllNode = i.second;
        while (dllNode != nullptr) {
            cout << (*Nodes)[dllNode->getNodeID()].getID() << "->";
            dllNode = dllNode->getNext();
        }
        cout << endl;
    }

    cout << "Right Bucket" << endl;
    for (auto i : rightBucket) {
        cout << "\tGain: " << i.first << "\t\t";
        linkedlist* dllNode = i.second;
        while (dllNode != nullptr) {
            cout << (*Nodes)[dllNode->getNodeID()].getID() << "->";
            dllNode = dllNode->getNext();
        }
        cout << endl;
    }

    cout << "Locked Nodes (In order): ";
    for (auto i : timeline) {
        cout << (*Nodes)[i.lockedNode].getID() << " ";
    }
    cout << endl;
}

int fmpass(vector<Node>* Nodes, vector<Net>* Nets) {
    // For all terminal nodes, ensure that they will no longer move.

    // Determine the gains, store the current gains in the Nodes structure.
    int cutsize = calculateCrossings(Nodes, Nets);

    // Creates the timeline necessary for FM to climb hills and work
    // The index will be the iteration number, while the timePoint struct holds the relevant information for that particular iteration
    vector<timePoint> timeline;
    int lowestCutsize = INT_MAX; // This will be used and updated as we find the lowest cutsize

    // Create 2 buckets -- left and right
    // This bucket is an unordered map (Hashmap), whose key is an int going from PMAX to -PMAX.
    // The value will be the pointer to a doubly linked list (DLL). The Data for this DLL will be an int, which will
    // reference the index in vector Nodes.
    map<int, linkedlist*> leftBucket; // Left bucket is considered 0
    map<int, linkedlist*> rightBucket; // Right bucket is considered 1
    map<int, linkedlist*>* chosenBucket;

    int lBucketSize = 0;
    int rBucketSize = 0;

    storeInBuckets(&leftBucket, &rightBucket, Nodes, &lBucketSize, &rBucketSize);
    if (logLevelGlobal > 2) {
        cout << "Left Size: " << lBucketSize << " R Size: " << rBucketSize << endl;
    }

    if (logLevelGlobal > 0) {
        printBuckets(leftBucket, rightBucket, -1, Nodes,timeline);
    }
    
    /*
        Hereinlies the FM algorithm
        Repeat the following until both LBucket and RBucket is empty
        Choose the side with the largest key (and thus the largest gain) Using myMap.begin()->first
        Loop to the end of the linked list
        Remove that cell from the bucket and place it on the timeline
        move that cell to the other side
        For all nets connected to that cell:
            For all nodes connected to that net:
                Recalculate the node crossings (This can be a -1 if the node is now on the same side, +1 if the node is now on the other side)
                Remove the node form the ebucket
                Place the node in the reclaulted gain bucket
    */
    int gainSelector = 0;
    int lastChosen = 0; //0 means left was the last chosen, 1 means the right was the last chosen.
    //-1 means this is our first time

    while (leftBucket.size() != 0 || rightBucket.size() != 0) {
        //If this is our first time, choose the side with the largest key
        if (lastChosen == -1) {
            // Choose the side with the largest key
            if (leftBucket.rbegin()->first - rightBucket.rbegin()->first >= 0) {
                if (logLevelGlobal > 2) {
                    cout << "Largest key is in left bucket ";
                }
                // Left bucket is bigger
                gainSelector = leftBucket.rbegin()->first;
                chosenBucket = &leftBucket;
                lastChosen = 0;
            } else {
                // Right bucket is bigger
                if (logLevelGlobal > 2) {
                    cout << "Largest key is in right bucket ";
                }
                gainSelector = rightBucket.rbegin()->first;
                chosenBucket = &rightBucket;
                lastChosen = 1;
            }
        } else {
            //This is not our first time.
            //Choose the side based on the last chosen -- essentially flip it
            if (lastChosen == 0) {
                if (logLevelGlobal > 2) {
                    cout << "Last Chosen was the left. Choosing right" << endl;
                }
                //choose the right bucket (As long as it's not empty)
                lastChosen = 1;
                if (rightBucket.empty()) {
                    if (logLevelGlobal > 2) {
                        cout << "Right bucket was empty, choosing left." << endl;
                    }
                    chosenBucket = &leftBucket;
                    gainSelector = leftBucket.rbegin()->first;
                } else {
                    chosenBucket = &rightBucket;
                    gainSelector = rightBucket.rbegin()->first;
                }
            } else {
                if (logLevelGlobal > 2) {
                    cout << "Last Chosen was the left. Choosing right" << endl;
                }
                //choose the left bucket (As long as it's not empty)
                lastChosen = 0;
                if (leftBucket.empty()) {
                    if (logLevelGlobal > 2) {
                        cout << "Left bucket was empty, choosing Right." << endl;
                    }
                    chosenBucket = &rightBucket;
                    gainSelector = rightBucket.rbegin()->first;
                } else {
                    chosenBucket = &leftBucket;
                    gainSelector = leftBucket.rbegin()->first;
                }
                
            }

        }

        

        if (logLevelGlobal > 2) {
	        cout << "with a gain of: " << gainSelector << endl;
        }
        
        // Loop to the end of the linked list
        linkedlist* dllNode = (*chosenBucket)[gainSelector];
        int selectedNode = -1;
        if (dllNode == nullptr) {
            cerr << "Error: dllNode is nullptr" << endl;
            exit(EXIT_FAILURE);
        }

        // Loop to the end of the node
        while (dllNode->getNext() != nullptr) {
            dllNode = dllNode->getNext();
        }
        // At this point we are at the last node
        selectedNode = (*dllNode).getNodeID(); 
        
        // linkedlist* prevNode = dllNode->getPrev();
        // if (prevNode != nullptr) {
        //     prevNode->setNext(nullptr);
        // }
        // delete dllNode; // Use delete instead of free for C++ objects

        struct timePoint point;
        point.lockedNode = selectedNode;
        point.cutSize = cutsize;
        point.ratio = getAreaRatio(*Nodes); 
        timeline.push_back(point);
        if (logLevelGlobal > 2) {
	        cout <<"cutsize before pass: " << cutsize << endl;
        }
        


        bool inc_cutsize, dec_cutsize; // flags that indicate if the cutsize should be incremented or decremented. 


        //Remove the the selectedNode out of the bucket structure
        if (logLevelGlobal > 2) {
	        cout << "Removing node[" << selectedNode << "]: " << (*Nodes)[selectedNode].getID() << " in bucket " << (*Nodes)[selectedNode].whichPartition() << " with gain of " << gainSelector << endl;
        }
        removeFromBucket(chosenBucket, gainSelector, selectedNode, Nodes);
        //Ensure that the removed node is locked
        (*Nodes)[selectedNode].lockNode();

        //Once removed, then update the parition
        (*Nodes)[selectedNode].movePartition();

        if (logLevelGlobal > 0) {
            printBuckets(leftBucket, rightBucket, selectedNode, Nodes,timeline);
        }
        
        
                
        
        // i refers to the index of the current net
        for (int i = 0; i < (*Nodes)[selectedNode].getConnectedNets().size(); i++) {
            int currentNet = (*Nodes)[selectedNode].getConnectedNets()[i];
            inc_cutsize = false;
            dec_cutsize=false;

            //j refers to the index of the current node
            for (int j = 0; j < (*Nets)[currentNet].getConnectedNodes().size(); j++) {
                // int oldGain = (2 * ((*Nodes)[j].getCrossings())) - (*Nodes)[j].getConnectedNets().size();

                //Looking for is the index of the node that we're now looking for
                int lookingFor = (*Nets)[currentNet].getConnectedNodes()[j];
                int oldGain = (2 * (*Nodes)[lookingFor].getCrossings()) - (*Nodes)[lookingFor].getConnectedNets().size();
                map<int, linkedlist*>* nodeUpdaterBucket;
                if (logLevelGlobal > 2) {
	                cout << "Starting search for node[" << lookingFor << "]: " << (*Nodes)[lookingFor].getID() << " with gain of " << oldGain << " = (2*" << (*Nodes)[lookingFor].getCrossings() << ") - " << (*Nodes)[lookingFor].getConnectedNets().size() << endl;
                }
                
                //This is the same node that we've now locked, skip
                //Also skip if the node is already locked
                if (lookingFor == selectedNode || (*Nodes)[lookingFor].getLockStatus() == 1) {
                    break;
                }
                
                if ((*Nodes)[lookingFor].whichPartition() == 1) {
                    //The node we are looking for is in the right bucket
                    nodeUpdaterBucket = &leftBucket;
                    if (logLevelGlobal > 2) {
	                    cout << "\t\tSearching left bucket" << endl;
                    }
                } else {
                    //The node are looking for is in the left bucket
                    nodeUpdaterBucket = &rightBucket;
                    if (logLevelGlobal > 2) {
	                    cout << "\t\tSearching right bucket" << endl;
                    }
                }

                // Find the jnode and remove it from the bucket structure
                // Loop to the end of the linked list
                linkedlist* nodeUpdater = (*nodeUpdaterBucket)[oldGain];
                if (nodeUpdater == nullptr) {
                    cerr << "Error: nodeUpdater is nullptr" << endl;
                    exit(EXIT_FAILURE);
                }

                // Loop until the end of the node
                // Or until we find the index we're looking for
                while (nodeUpdater->getNodeID() != lookingFor) {
                    if (logLevelGlobal > 2) {
	                    cout <<"\t\tCurrently at node[" << nodeUpdater->getNodeID() << "]: " << (*Nodes)[nodeUpdater->getNodeID()].getID() << endl;
                    }
                    if (nodeUpdater->getNext() == nullptr) {
                        //If this triggers, we are at the end of the list
                        cerr << "Error: nodeUpdater->getNext() is nullptr" << endl;
                        exit(EXIT_FAILURE);
                    }
                    nodeUpdater = nodeUpdater->getNext();
                }
                // nodeUpdater now has the node we're trying to move
                // Update the DLL to remove nodeUpdater from the area

                // If nodeUpdater.getPrev() is nullptr, it is the first.
                // If nodeUpdater.getNext() is nullptr, it is the last.
                // If it is both, remove the key entirely from the map
                linkedlist* nodeUpdaterPrev = nodeUpdater->getPrev();
                linkedlist* nodeUpdaterNext = nodeUpdater->getNext();

                // Set nodeupdater to default nullptrs for both
                nodeUpdater->setPrev(nullptr);
                nodeUpdater->setNext(nullptr);

                if (nodeUpdaterPrev == nullptr && nodeUpdaterNext == nullptr) {
                    (*nodeUpdaterBucket).erase(oldGain);
                    if (logLevelGlobal > 2) {
	                    cout << "\t\tRemoving key from bucket" << endl;
                    }
                } else if (nodeUpdaterPrev == nullptr && nodeUpdaterNext != nullptr) {
                    // This is the first node, link this to the head in the key
                    (*nodeUpdaterBucket)[oldGain] = nodeUpdaterNext;
                    nodeUpdaterNext->setPrev(nullptr);
                    if (logLevelGlobal > 2) {
	                    cout << "\t\tGet the next node and move it to the head" << endl;
                    }
                } else if (nodeUpdaterPrev != nullptr && nodeUpdaterNext == nullptr) {
                    // This is the last node, simply remove the previous connection
                    nodeUpdaterPrev->setNext(nullptr);
                    if (logLevelGlobal > 2) {
	                    cout << "\t\tThis is the last node, set prev to null" << endl;
                    }
                } else {
                    if (logLevelGlobal > 2) {
	                    cout << "\t\tFound in the middle" << endl;
                    }
                    // Standard case, simply stitch the DLL back together
                    nodeUpdaterPrev->setNext(nodeUpdaterNext);
                    nodeUpdaterNext->setPrev(nodeUpdaterPrev);
                }

                if ((*Nodes)[selectedNode].whichPartition() ^ (*Nodes)[lookingFor].whichPartition()) {
                    //They are different sides
                    // a net will only be cut when it goes up from a 0 to 1. 
                    if ((*Nodes)[lookingFor].getCrossings() == 0){inc_cutsize = true;}
                    (*Nodes)[lookingFor].incCrossings();

                } else {
                    //they are same sides
                    // a net can only be uncut when the number of crossing go down from a 1 to a 0. 
                    if((*Nodes)[lookingFor].getCrossings() == 1){dec_cutsize = true; }
                    (*Nodes)[lookingFor].decCrossings();
                }
                int newGain = 2 * ((*Nodes)[lookingFor].getCrossings()) - (*Nodes)[lookingFor].getConnectedNets().size();
                if (logLevelGlobal > 2) {
	                cout << "\t\tNew Gain: "<< newGain << endl;
                }

                // Place the selected node back into the same bucket with the new gain
                if ((*nodeUpdaterBucket).find(newGain) != (*nodeUpdaterBucket).end()) {
                    // If the key exists, add it to the beginning of the linked list
                    nodeUpdaterNext = (*nodeUpdaterBucket)[newGain]; // Here, I'm reusing nodeUpdaterNext
                    (*nodeUpdaterBucket)[newGain] = nodeUpdater;
                    nodeUpdater->setNext(nodeUpdaterNext);
                    nodeUpdaterNext->setPrev(nodeUpdater);
                    if (logLevelGlobal > 2) {
	                    cout << "\t\tKey exists, placed back in bucket with gain: " << newGain << endl;
                    }

                } else {
                    // If the key does not exist, add it to the map
                    (*nodeUpdaterBucket)[newGain] = nodeUpdater;
                    if (logLevelGlobal > 2) {
                        cout << "\t\tKey does not exist, creating: " << newGain << endl;
                        cout << "\t\t" << (*nodeUpdaterBucket)[newGain]->getNodeID() << endl;
                    }  
                }
            }
            if(inc_cutsize){cutsize++;}
            if(dec_cutsize){cutsize--;}
        }
        if (logLevelGlobal > 2) {
	        cout<< "cutsize after the pass: " << cutsize << endl; 
        }
        if (cutsize < lowestCutsize)
            lowestCutsize = cutsize;
    }

    if (logLevelGlobal > 1) {
	    printBuckets(leftBucket, rightBucket, -1, Nodes,timeline);
    }

    if (logLevelGlobal > 2) {
	    cout << "Buckets are empty" << endl;
    }
// Here, all cells are now fixed and emptied out of the buckets.
    // Roll back the changes from the timeline one by one until we've reached the lowest cutsize during this iteration
    // Lowest cutsize is found in variable lowestCutsize, so just keep rolling back until we've gotten to a point in history
    // Where that is the case
    for (int i = timeline.size() - 1; i >= 0; i--) {
        if (timeline[i].cutSize == lowestCutsize) {
            break;
        }
        // Otherwise flip the node and continue
        (*Nodes)[timeline[i].lockedNode].movePartition();
    }

    // This is the most efficient cutsize of the current FM pass
    cout << "Final Cut size: "<< lowestCutsize << endl;
    return lowestCutsize;
    
}

int FM(vector<Node>* Nodes, vector<Net>* Nets) {
    int cut = INT_MAX;
    int lastCut = INT_MAX;

    do{
        lastCut = cut;
        cut = fmpass(Nodes, Nets);
        cout << endl << endl;
    } while (cut < lastCut);

    return lastCut;
}