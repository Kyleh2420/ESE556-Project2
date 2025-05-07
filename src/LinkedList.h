#ifndef LinkedList_H
#define LinkedList_H

#include "Node.h"

class linkedlist {
    public:
        int node_id_idx; // Store index of the node in the main Nodes vector
        linkedlist* prev;
        linkedlist* next;
    
        linkedlist(int id_idx) : node_id_idx(id_idx), prev(nullptr), next(nullptr) {}
        int getNodeID() const { return node_id_idx; }
        linkedlist* getNext() const { return next; }
        void setNext(linkedlist* n) { next = n; }
        linkedlist* getPrev() const { return prev; }
        void setPrev(linkedlist* p) { prev = p; }
    };

#endif