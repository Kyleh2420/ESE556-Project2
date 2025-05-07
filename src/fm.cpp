#include <iostream>
#include <vector>
#include <map>
#include <algorithm> // For std::max, std::min
#include <limits>    // For std::numeric_limits
#include <string>    // From your Node.h/Net.h
#include <utility>   // For std::pair (if not already included by map)
#include <cmath>     // For std::abs

// Include your actual Node and Net header files
#include "Node.h"
#include "Net.h"

// If LinkedList.h is a separate file you have, include it.
// Otherwise, the definition from the previous artifact is used below.
// #include "LinkedList.h"


// --- ASSUMPTIONS about additions to your classes ---
/*
In Node.h (inside Node class):
public:
    int gain; // To store calculated gain
    int getGain() const { return gain; }
    void setGain(int g) { gain = g; }

In Net.h (inside Net class):
public:
    std::pair<int, int> distribution; // <count_in_partition_0, count_in_partition_1>

    void updateDistribution(const std::vector<Node>& all_nodes) {
        distribution.first = 0;
        distribution.second = 0;
        for (int node_idx : connectedNodes_) { 
            if (node_idx < 0 || node_idx >= all_nodes.size()) {
                // Add error handling or skip if index is out of bounds
                // std::cerr << "Warning: Node index " << node_idx << " out of bounds for net " << getName() << std::endl;
                continue;
            }
            const Node& node = all_nodes[node_idx]; // Ensure const correctness if possible
            if (node.isTerminal()) continue;
            if (node.whichPartition() == 0) { 
                distribution.first++;
            } else {
                distribution.second++;
            }
        }
    }
    std::pair<int, int> getDistribution() const { return distribution; }

    // Constructor might need to initialize distribution
    // Net(string netName, vector<int> net_node_indices) : netName_(netName), connectedNodes_(net_node_indices), distribution({0,0}) {}
*/


// // Your Doubly Linked List for gain buckets
// class linkedlist {
// public:
//     int node_id_idx; 
//     linkedlist* prev;
//     linkedlist* next;

//     linkedlist(int id_idx) : node_id_idx(id_idx), prev(nullptr), next(nullptr) {}
//     int getNodeID() const { return node_id_idx; }
//     linkedlist* getNext() const { return next; }
//     void setNext(linkedlist* n) { next = n; }
//     linkedlist* getPrev() const { return prev; }
//     void setPrev(linkedlist* p) { prev = p; }
// };

struct timePoint {
    int lockedNode_idx; 
    int cutSize;
    float ratio_part0; 
};

// --- Helper functions for bucket management ---
void removeFromGainList(std::map<int, linkedlist*>& bucket, int gain_val, int node_idx_to_remove) {
    auto it = bucket.find(gain_val);
    if (it == bucket.end()) return;

    linkedlist* current = it->second;
    linkedlist* head = current; 

    while (current != nullptr && current->getNodeID() != node_idx_to_remove) {
        current = current->getNext();
    }

    if (current == nullptr) return; 

    if (current->getPrev() != nullptr) {
        current->getPrev()->setNext(current->getNext());
    } else { 
        head = current->getNext(); 
    }

    if (current->getNext() != nullptr) {
        current->getNext()->setPrev(current->getPrev());
    }

    delete current; 

    if (head == nullptr) { 
        bucket.erase(it); // Use iterator to erase for efficiency
    } else {
        it->second = head; 
    }
}

void addToGainList(std::map<int, linkedlist*>& bucket, int gain_val, int node_idx_to_add) {
    linkedlist* new_ll_node = new linkedlist(node_idx_to_add);
    auto it = bucket.find(gain_val);
    if (it == bucket.end()) {
        bucket[gain_val] = new_ll_node;
    } else {
        linkedlist* head = it->second;
        new_ll_node->setNext(head);
        if (head) head->setPrev(new_ll_node);
        it->second = new_ll_node;
    }
}


// --- Main fmpass function ---
// IMPORTANT: Ensure balance_factor_lower_bound and balance_factor_upper_bound are restrictive
// (e.g., 0.4 and 0.6) to prevent all nodes moving to one partition.
int fmpass(std::vector<Node>& all_nodes, std::vector<Net>& all_nets,
           float balance_factor_lower_bound, float balance_factor_upper_bound,
           int global_num_nodes 
          ) {

    float total_node_area = 0;
    int num_free_cells = 0;
    for(int i = 0; i < global_num_nodes; ++i) {
        if (i >= all_nodes.size()) continue; // Basic bounds check
        if (!all_nodes[i].isTerminal()) {
            total_node_area += all_nodes[i].getArea();
            num_free_cells++;
        }
    }

    if (num_free_cells == 0 || total_node_area <= 0) { // Added total_node_area check
        // Calculate cutsize for an empty/all-terminal graph if necessary, or return 0
        int initial_cut = 0;
        for(Net& net : all_nets){
            net.updateDistribution(all_nodes); // Ensure distributions are up-to-date
            std::pair<int,int> dist = net.getDistribution();
            if(dist.first > 0 && dist.second > 0) initial_cut++;
        }
        return initial_cut;
    }


    std::map<int, linkedlist*> left_bucket_list;  // Partition 0 (false)
    std::map<int, linkedlist*> right_bucket_list; // Partition 1 (true)
    
    float current_area_part0 = 0;
    for (int i = 0; i < global_num_nodes; ++i) {
        if (i >= all_nodes.size()) continue;
        if (!all_nodes[i].isTerminal()) {
            all_nodes[i].unlockNode(); 
            if (all_nodes[i].whichPartition() == 0) { 
                current_area_part0 += all_nodes[i].getArea(); 
            }
        }
    }

    for (Net& net : all_nets) {
        net.updateDistribution(all_nodes);
    }

    int current_cutsize = 0;
    for (Net& net : all_nets) { 
        std::pair<int, int> dist = net.getDistribution(); 
        if (dist.first > 0 && dist.second > 0) {
            current_cutsize++;
        }
    }

    for (int i = 0; i < global_num_nodes; ++i) {
        if (i >= all_nodes.size()) continue;
        Node& node_c = all_nodes[i];
        if (node_c.isTerminal()) continue;

        int calculated_gain = 0;
        for (int net_idx : node_c.getConnectedNets()) { 
            if (net_idx < 0 || net_idx >= all_nets.size()) continue; // Bounds check
            Net& net = all_nets[net_idx]; 
            std::pair<int, int> dist = net.getDistribution();
            
            int num_cells_in_c_part = (node_c.whichPartition() == 0) ? dist.first : dist.second;
            int num_cells_in_other_part = (node_c.whichPartition() == 0) ? dist.second : dist.first;

            if (num_cells_in_c_part == 1 && num_cells_in_other_part > 0) calculated_gain++; 
            if (num_cells_in_other_part == 0 && num_cells_in_c_part > 0) calculated_gain--; // Corrected TE: if it's the only one keeping the net from being cut (all on its side)
        }
        node_c.setGain(calculated_gain); 

        if (node_c.whichPartition() == 0) {
            addToGainList(left_bucket_list, node_c.getGain(), i); 
        } else {
            addToGainList(right_bucket_list, node_c.getGain(), i);
        }
    }
    
    std::vector<timePoint> timeline;
    int min_cutsize_this_pass = current_cutsize;
    int best_move_index_this_pass = 0; // Initialize to 0 for the initial state

    timePoint initial_tp;
    initial_tp.lockedNode_idx = -1; 
    initial_tp.cutSize = current_cutsize;
    initial_tp.ratio_part0 = current_area_part0 / total_node_area;
    timeline.push_back(initial_tp);

    for (int move_iter = 0; move_iter < num_free_cells; ++move_iter) {
        int base_cell_idx = -1;
        int best_gain_so_far = std::numeric_limits<int>::min();
        float best_move_balance_metric = std::numeric_limits<float>::max(); // Lower is better (closer to 0.5)
        // bool selected_move_from_left = false; // Not strictly needed if base_cell_idx is set

        // Find best legal move from left_bucket_list (partition 0 to 1)
        if (!left_bucket_list.empty()) {
            // Iterate from highest gain downwards. std::map iterates key ascending. rbegin() gives largest key.
            for (auto it = left_bucket_list.rbegin(); it != left_bucket_list.rend(); ++it) {
                int current_gain_in_list = it->first;
                if (current_gain_in_list < best_gain_so_far && base_cell_idx != -1) break; // Optimization

                linkedlist* current_ll_node = it->second;
                while(current_ll_node != nullptr) {
                    int potential_node_idx = current_ll_node->getNodeID();
                    if (potential_node_idx < 0 || potential_node_idx >= all_nodes.size()) {
                        current_ll_node = current_ll_node->getNext(); continue; // Bounds check
                    }
                    Node& p_node = all_nodes[potential_node_idx];

                    if (p_node.isLocked()) {
                        current_ll_node = current_ll_node->getNext();
                        continue;
                    }
                    // Ensure we are only considering nodes with the current_gain_in_list
                    if (p_node.getGain() != current_gain_in_list) {
                         current_ll_node = current_ll_node->getNext();
                         continue;
                    }


                    float area_node = p_node.getArea();
                    float next_area_part0 = current_area_part0 - area_node; 
                    float ratio0 = next_area_part0 / total_node_area;
                    float current_move_abs_balance_from_center = std::abs(ratio0 - 0.5f);

                    if (ratio0 >= balance_factor_lower_bound && ratio0 <= balance_factor_upper_bound) {
                        if (p_node.getGain() > best_gain_so_far) {
                            best_gain_so_far = p_node.getGain();
                            base_cell_idx = potential_node_idx;
                            best_move_balance_metric = current_move_abs_balance_from_center;
                        } else if (p_node.getGain() == best_gain_so_far) {
                            if (current_move_abs_balance_from_center < best_move_balance_metric) { 
                                base_cell_idx = potential_node_idx;
                                best_move_balance_metric = current_move_abs_balance_from_center;
                            }
                        }
                    }
                    current_ll_node = current_ll_node->getNext();
                }
            }
        }

        // Find best legal move from right_bucket_list (partition 1 to 0)
         if (!right_bucket_list.empty()) {
            for (auto it = right_bucket_list.rbegin(); it != right_bucket_list.rend(); ++it) {
                int current_gain_in_list = it->first;
                 // Compare with overall best_gain_so_far before iterating list
                if (current_gain_in_list < best_gain_so_far && base_cell_idx != -1) break;
                // If current_gain_in_list == best_gain_so_far, we might still find a better balanced move
                if (current_gain_in_list == best_gain_so_far && best_move_balance_metric == 0.0f && base_cell_idx !=-1) break; // Perfect balance found at this gain

                linkedlist* current_ll_node = it->second;
                while(current_ll_node != nullptr) {
                    int potential_node_idx = current_ll_node->getNodeID();
                     if (potential_node_idx < 0 || potential_node_idx >= all_nodes.size()) {
                        current_ll_node = current_ll_node->getNext(); continue; // Bounds check
                    }
                    Node& p_node = all_nodes[potential_node_idx];

                    if (p_node.isLocked()) {
                        current_ll_node = current_ll_node->getNext();
                        continue;
                    }
                    if (p_node.getGain() != current_gain_in_list) {
                         current_ll_node = current_ll_node->getNext();
                         continue;
                    }


                    float area_node = p_node.getArea();
                    float next_area_part0 = current_area_part0 + area_node; 
                    float ratio0 = next_area_part0 / total_node_area;
                    float current_move_abs_balance_from_center = std::abs(ratio0 - 0.5f);
                    
                    if (ratio0 >= balance_factor_lower_bound && ratio0 <= balance_factor_upper_bound) {
                         if (p_node.getGain() > best_gain_so_far) {
                            best_gain_so_far = p_node.getGain();
                            base_cell_idx = potential_node_idx;
                            best_move_balance_metric = current_move_abs_balance_from_center;
                        } else if (p_node.getGain() == best_gain_so_far) {
                            if (current_move_abs_balance_from_center < best_move_balance_metric) {
                                base_cell_idx = potential_node_idx;
                                best_move_balance_metric = current_move_abs_balance_from_center;
                            }
                        }
                    }
                    current_ll_node = current_ll_node->getNext();
                }
            }
        }

        if (base_cell_idx == -1) break; 

        Node& moving_cell = all_nodes[base_cell_idx];
        int old_gain_of_moving_cell = moving_cell.getGain();
        bool old_partition_bool = moving_cell.whichPartition(); 
        
        if (old_partition_bool == 0) { 
            removeFromGainList(left_bucket_list, old_gain_of_moving_cell, base_cell_idx);
            current_area_part0 -= moving_cell.getArea();
        } else { 
            removeFromGainList(right_bucket_list, old_gain_of_moving_cell, base_cell_idx);
            current_area_part0 += moving_cell.getArea(); 
        }
        
        moving_cell.lockNode(); 
        moving_cell.movePartition(); 
        
        for (int net_idx : moving_cell.getConnectedNets()) {
            if (net_idx < 0 || net_idx >= all_nets.size()) continue;
            Net& current_net = all_nets[net_idx];
            
            std::pair<int, int> dist_now = current_net.getDistribution(); 
            int cells_in_part0_before_move = dist_now.first;
            int cells_in_part1_before_move = dist_now.second;

            if (old_partition_bool == 0) { 
                cells_in_part0_before_move++; 
                cells_in_part1_before_move--; 
            } else { 
                cells_in_part1_before_move++;
                cells_in_part0_before_move--;
            }
            // Ensure counts are not negative if logic is subtle
            cells_in_part0_before_move = std::max(0, cells_in_part0_before_move);
            cells_in_part1_before_move = std::max(0, cells_in_part1_before_move);


            bool was_cut = (cells_in_part0_before_move > 0 && cells_in_part1_before_move > 0);
            current_net.updateDistribution(all_nodes); 
            std::pair<int, int> dist_after_move = current_net.getDistribution();
            bool is_cut = (dist_after_move.first > 0 && dist_after_move.second > 0);

            if (was_cut && !is_cut) current_cutsize--;
            if (!was_cut && is_cut) current_cutsize++;
        }
        
        timePoint tp;
        tp.lockedNode_idx = base_cell_idx;
        tp.cutSize = current_cutsize;
        tp.ratio_part0 = current_area_part0 / total_node_area;
        timeline.push_back(tp);

        if (current_cutsize < min_cutsize_this_pass) {
           min_cutsize_this_pass = current_cutsize;
           best_move_index_this_pass = timeline.size() - 1; 
        } else if (current_cutsize == min_cutsize_this_pass) {
            // Tie-break for best_move_index_this_pass based on balance
            if (best_move_index_this_pass < timeline.size()){ // Ensure index is valid
                 float current_balance_abs = std::abs(tp.ratio_part0 - 0.5f);
                 float best_recorded_balance_abs = std::abs(timeline[best_move_index_this_pass].ratio_part0 - 0.5f);
                 if (current_balance_abs < best_recorded_balance_abs) {
                     best_move_index_this_pass = timeline.size() -1;
                 }
            }
        }

        for (int net_idx : moving_cell.getConnectedNets()) {
            if (net_idx < 0 || net_idx >= all_nets.size()) continue;
            Net& current_net = all_nets[net_idx]; 
            
            int cells_in_P_old_before_move; 
            int cells_in_P_new_before_move; 

            // This logic determines the state of the net *before* the `moving_cell` was moved,
            // from the perspective of the `moving_cell`'s original partition (P_old)
            // and its target partition (P_new).
            if (old_partition_bool == 0) { // moving_cell was in P0 (old), moved to P1 (new)
                cells_in_P_old_before_move = current_net.getDistribution().first + 1; 
                cells_in_P_new_before_move = current_net.getDistribution().second - 1;
            } else { // moving_cell was in P1 (old), moved to P0 (new)
                cells_in_P_old_before_move = current_net.getDistribution().second + 1;
                cells_in_P_new_before_move = current_net.getDistribution().first - 1;
            }
            cells_in_P_old_before_move = std::max(0, cells_in_P_old_before_move);
            cells_in_P_new_before_move = std::max(0, cells_in_P_new_before_move);

            for (int neighbor_node_idx : current_net.getConnectedNodes()) { 
                if (neighbor_node_idx < 0 || neighbor_node_idx >= all_nodes.size()) continue;
                if (neighbor_node_idx == base_cell_idx || all_nodes[neighbor_node_idx].isLocked() || all_nodes[neighbor_node_idx].isTerminal()) {
                    continue;
                }
                Node& neighbor_node = all_nodes[neighbor_node_idx];
                int old_neighbor_gain = neighbor_node.getGain(); 
                int gain_delta = 0;

                if (neighbor_node.whichPartition() == old_partition_bool) { 
                    if (cells_in_P_new_before_move == 0) { 
                        gain_delta++;
                    }
                    // Criticality check for decrement (more complex, from original FM):
                    // If net N had all its cells in P_old (i.e. cells_in_P_new_before_move == 0)
                    // and after moving_cell, net N becomes critical for neighbor_node in P_old
                    // (i.e. cells_in_P_old_after_move == 1, and that one is neighbor_node)
                    // then gain_delta--; (This is a common rule, simplified here)
                    int cells_in_P_old_after_move = current_net.getDistribution().first; // if old_partition_bool == 0
                    if(old_partition_bool == 1) cells_in_P_old_after_move = current_net.getDistribution().second;

                    if (cells_in_P_old_after_move == 0 && cells_in_P_old_before_move > 0) { // Net just became empty in P_old from neighbor's perspective
                         // This implies moving_cell was the last one along with potentially others, and now it's gone.
                         // If neighbor_node was previously not critical for FS because moving_cell was also there,
                         // and now neighbor_node IS critical for FS, its gain might change.
                         // The FS/TE logic is more precise:
                         // Recalculate FS(neighbor) and TE(neighbor) for this net N.
                         // For this simplified delta:
                         // If net N was NOT critical for neighbor_node (because moving_cell was also in P_old on net N)
                         // and NOW net N IS critical for neighbor_node (because moving_cell left P_old)
                         // then gain_delta--; (if moving neighbor now cuts the net, which it would if it was previously all in P_old)
                    }


                } else { // Neighbor is in P_new (partition base_cell joined)
                     if (cells_in_P_old_before_move == 1) {
                        gain_delta++;
                     }
                     // Criticality check for decrement:
                     // If net N was critical for neighbor_node in P_new (i.e. cells_in_P_new_before_move == 1 and it was neighbor_node)
                     // and after moving_cell, net N is no longer critical for neighbor_node in P_new (because moving_cell joined)
                     // then gain_delta--;
                     if(cells_in_P_new_before_move == 0 && current_net.getDistribution().second > 1 && old_partition_bool == 0){ // neighbor in P1, moving_cell came to P1
                        // If neighbor was critical (FS), and now moving_cell also in P1, neighbor is no longer critical for FS on this net.
                        // gain_delta--;
                     }
                     if(cells_in_P_new_before_move == 0 && current_net.getDistribution().first > 1 && old_partition_bool == 1){ // neighbor in P0, moving_cell came to P0
                        // gain_delta--;
                     }
                }
                
                if (gain_delta != 0) {
                    std::map<int, linkedlist*>& relevant_bucket = (neighbor_node.whichPartition() == 0) ? left_bucket_list : right_bucket_list;
                    removeFromGainList(relevant_bucket, old_neighbor_gain, neighbor_node_idx);
                    neighbor_node.setGain(old_neighbor_gain + gain_delta); 
                    addToGainList(relevant_bucket, neighbor_node.getGain(), neighbor_node_idx);
                }
            }
        }
    }

    int rollback_start_idx = timeline.size() - 1;
    // best_move_index_this_pass is an index for timeline.
    // timeline[0] is the initial state.
    // If best_move_index_this_pass is 0, it means initial state was best or no improvement.

    for (int i = rollback_start_idx; i > best_move_index_this_pass ; --i) {
        if (i < timeline.size() && timeline[i].lockedNode_idx != -1 && 
            timeline[i].lockedNode_idx < all_nodes.size()) { 
            all_nodes[timeline[i].lockedNode_idx].movePartition(); 
        }
    }
    
    if (best_move_index_this_pass >=0 && best_move_index_this_pass < timeline.size()) {
         // After rollback, update distributions one last time to be sure
         for(Net& net : all_nets) net.updateDistribution(all_nodes);
         return timeline[best_move_index_this_pass].cutSize;
    }
    
    // Should not be reached if timeline always has at least initial_tp
    return current_cutsize; 
}

// Example main or calling function structure (illustrative)
/*
int main() {
    // --- Example Setup ---
    std::vector<Node> nodes;
    nodes.emplace_back("c1", 10, 1, false); nodes.back().setPartition(0); // idx 0
    nodes.emplace_back("c2", 10, 1, false); nodes.back().setPartition(0); // idx 1
    nodes.emplace_back("c3", 10, 1, false); nodes.back().setPartition(1); // idx 2
    nodes.emplace_back("c4", 10, 1, false); nodes.back().setPartition(1); // idx 3
    int num_total_nodes = nodes.size();

    std::vector<Net> nets;
    nets.emplace_back("n1", std::vector<int>{0, 2}); 
    nets.emplace_back("n2", std::vector<int>{1, 3}); 
    nets.emplace_back("n3", std::vector<int>{0, 1}); 
    nets.emplace_back("n4", std::vector<int>{2, 3}); 

    nodes[0].addNet(0); nodes[0].addNet(2); 
    nodes[1].addNet(1); nodes[1].addNet(2); 
    nodes[2].addNet(0); nodes[2].addNet(3); 
    nodes[3].addNet(1); nodes[3].addNet(3); 
    
    // IMPORTANT: Ensure Node has setGain/getGain and Net has updateDistribution/getDistribution
    // and their constructors initialize new members (e.g. Net::distribution to {0,0})

    // CRITICAL: Set these to sensible values, e.g., 0.4 to 0.6, NOT 0.0 to 1.0
    float balance_low = 0.40f; 
    float balance_high = 0.60f; 
    
    std::cout << "Initial state:" << std::endl;
    float initial_area_p0 = 0;
    float total_area = 0;
    for(int i=0; i<num_total_nodes; ++i) {
        if(!nodes[i].isTerminal()) total_area += nodes[i].getArea();
        if(!nodes[i].isTerminal() && nodes[i].whichPartition()==0) initial_area_p0 += nodes[i].getArea();
        // std::cout << "Node " << nodes[i].getID() << " partition: " << nodes[i].whichPartition() << std::endl;
    }
     int initial_cut = 0;
     for(Net& net : nets){
         net.updateDistribution(nodes);
         std::pair<int,int> dist = net.getDistribution();
         if(dist.first > 0 && dist.second > 0) initial_cut++;
     }
    std::cout << "Initial Cutsize: " << initial_cut << std::endl;
    std::cout << "Initial P0 Ratio: " << (total_area > 0 ? initial_area_p0/total_area : 0) << std::endl;

    
    int final_cutsize = fmpass(nodes, nets, balance_low, balance_high, num_total_nodes);
    std::cout << "Final cutsize after one FM pass: " << final_cutsize << std::endl;

    std::cout << "Final state:" << std::endl;
    float final_area_p0 = 0;
    for(int i=0; i<num_total_nodes; ++i) {
         if(!nodes[i].isTerminal() && nodes[i].whichPartition()==0) final_area_p0 += nodes[i].getArea();
        // std::cout << "Node " << nodes[i].getID() << " partition: " << nodes[i].whichPartition() << std::endl;
    }
    std::cout << "Final P0 Ratio: " << (total_area > 0 ? final_area_p0/total_area : 0) << std::endl;
    // Remember to delete linkedlist nodes from buckets if running multiple passes or in a larger application context.
    return 0;
}
*/
