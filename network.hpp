//
//  network.hpp
//  Traffic-Routing
//
//  Created by 张沈麒 on 1/28/20.
//  Copyright © 2020 张沈麒. All rights reserved.
//

#ifndef network_h
#define network_h

#include "vertex.hpp"
#include "packet.hpp"
#include <vector>
using namespace std;

enum PathMode {efficient_path, quickest_path};

class network {
public:
    network(size_t n);
    network(size_t n, vector<vector<size_t>> edges);
    virtual ~network();
    size_t get_vertex_sum();
    size_t get_packet_queue_size(size_t id);
    void push_packet_queue(size_t id, packet *ptr);
    packet *poll_packet_queue(size_t id);
    vector<size_t> get_shortest_path(size_t source, size_t destination);
    vector<size_t> get_efficient_path(size_t source, size_t destination);
    vector<size_t> get_quickest_path(size_t source, size_t destination);
    void print();
protected:
    size_t vertex_sum;
    vector<vertex*> adjacency_list;
private:
    size_t get_neighbor_distance(size_t neighbor, PathMode mode_type);
    vector<size_t> get_path_dijkstra(size_t source, size_t destination, PathMode mode_type);
};

class lattice_network : public network {
public:
    lattice_network(size_t size);
    ~lattice_network();
};

class BA_network : public network {
public:
    BA_network(size_t n0, size_t nt, size_t nc);
    ~BA_network();
};

#endif /* network_h */
