//
//  network.cpp
//  Traffic-Routing
//
//  Created by 张沈麒 on 1/28/20.
//  Copyright © 2020 张沈麒. All rights reserved.
//

#include "network.hpp"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <utility>

network::network(size_t n) {
//    cout << "Constructor network" << endl;
    vertex_sum = n;
    adjacency_list.resize(vertex_sum);
    for (size_t i = 0; i < vertex_sum; i++) {
        vertex *v = new vertex();
        adjacency_list[i] = v;
    }
}

network::network(size_t n, vector<vector<size_t>> edges) : network(n) {
//    cout << "Constructor network" << endl;
    for (vector<size_t> edge: edges) {
        size_t v1 = edge[0], v2 = edge[1];
        if (v1 < 0 || v1 >= vertex_sum || v2 < 0 || v2 >= vertex_sum) {
            throw runtime_error("Error: Node id is out of range!");
        }
        adjacency_list[v1]->neighbors.push_front(v2);
        adjacency_list[v1]->degree++;
        adjacency_list[v2]->neighbors.push_front(v1);
        adjacency_list[v2]->degree++;
    }
}

network::~network() {
//    cout << "Destructor network" << endl;
    for (size_t i = 0; i < vertex_sum; i++) {
        delete adjacency_list[i];
    }
}

size_t network::get_vertex_sum() {
    return vertex_sum;
}

size_t network::get_packet_queue_size(size_t id) {
    if (id >= vertex_sum) {
        throw runtime_error("Error: Node id is out of range!");
    }
    return adjacency_list[id]->packet_queue.size();
}

void network::push_packet_queue(size_t id, packet *ptr) {
    if (id >= vertex_sum) {
        throw runtime_error("Error: Node id is out of range!");
    }
    adjacency_list[id]->packet_queue.push(ptr);
}

packet *network::poll_packet_queue(size_t id) {
    if (id >= vertex_sum) {
        throw runtime_error("Error: Node id is out of range!");
    }
    packet *ptr = adjacency_list[id]->packet_queue.front();
    adjacency_list[id]->packet_queue.pop();
    return ptr;
}

vector<size_t> network::get_shortest_path(size_t source, size_t destination) {
    if (source >= vertex_sum || destination >= vertex_sum) {
        throw runtime_error("Error: Node id is out of range!");
    }
    
    if (source == destination) {
        vector<size_t> path {source};
        return path;
    }
    
    size_t distance = 1;
    unordered_map<size_t, size_t> prev_vertex;
    prev_vertex[source] = source;
    queue<size_t> q;
    q.push(source);
    while (!q.empty()) {
        distance++;
        auto size = q.size();
        for (auto i = 0; i < size; i++) {
            size_t curr = q.front();
            q.pop();
            for (size_t neighbor: adjacency_list[curr]->neighbors) {
                if (prev_vertex.find(neighbor) == prev_vertex.end()) {
                    prev_vertex[neighbor] = curr;
                    if (neighbor == destination) {
                        vector<size_t> path(distance, 0);
                        curr = destination;
                        for (size_t j = distance - 1; j < SIZE_MAX; j--) {
                            path[j] = curr;
                            curr = prev_vertex[curr];
                        }
                        return path;
                    } else {
                        q.push(neighbor);
                    }
                }
            }
        }
    }
    throw runtime_error("Error: No path is found!");
}

vector<size_t> network::get_efficient_path(size_t source, size_t destination) {
    return get_path_dijkstra(source, destination, efficient_path);
}

vector<size_t> network::get_quickest_path(size_t source, size_t destination) {
    return get_path_dijkstra(source, destination, quickest_path);
}

size_t network::get_neighbor_distance(size_t neighbor, PathMode mode_type) {
    switch (mode_type) {
        case efficient_path:
            return adjacency_list[neighbor]->degree;
            
        case quickest_path:
            return adjacency_list[neighbor]->packet_queue.size() + 1;
            
        default:
            throw runtime_error("Error: Path mode is invalid!");
    }
}

vector<size_t> network::get_path_dijkstra(size_t source, size_t destination, PathMode mode_type) {
    if (source >= vertex_sum || destination >= vertex_sum) {
        throw runtime_error("Error: Node id is out of range!");
    }
    
    if (source == destination) {
        vector<size_t> path{source};
        return path;
    }
    
    unordered_map<size_t, size_t> prev_vertex;
    prev_vertex[source] = source;
    vector<size_t> distance (vertex_sum, SIZE_MAX);
    distance[source] = 0;
    priority_queue<pair<size_t, size_t>, vector<pair<size_t, size_t>>, greater<pair<size_t, size_t>>> pq;
    pq.push(make_pair(0, source));
    while (!pq.empty()) {
        size_t curr = pq.top().second;
        if (curr == destination) {
            size_t vertex_count = 1;
            while (curr != source) {
                vertex_count++;
                curr = prev_vertex[curr];
            }
            curr = destination;
            vector<size_t> path (vertex_count, 0);
            for (size_t i = vertex_count - 1; i < SIZE_MAX; i--) {
                path[i] = curr;
                curr = prev_vertex[curr];
            }
            return path;
        }
        pq.pop();
        for (size_t neighbor: adjacency_list[curr]->neighbors) {
            size_t new_distance = distance[curr] + get_neighbor_distance(neighbor, mode_type);
            if (new_distance < distance[neighbor]) {
                distance[neighbor] = new_distance;
                prev_vertex[neighbor] = curr;
                pq.push(make_pair(new_distance, neighbor));
            }
        }
    }
    throw runtime_error("Error: No path is found!");
}

void network::print() {
    for (size_t i = 0; i < vertex_sum; i++) {
        cout << "id: " << i << "\t";
        adjacency_list[i]->print();
    }
}

lattice_network::lattice_network(size_t size) : network(size * size) {
//    cout << "Constructor lattice_network" << endl;
    if (size > (size_t)sqrt(SIZE_MAX)) {
        throw runtime_error("Error: Number of vertexes is out of range!");
    }
    
    for (size_t row = size - 2; row < SIZE_MAX; row--) {
        for (size_t col = size - 2; col < SIZE_MAX; col--) {
            size_t v1 = row * size + col;
            size_t v2 = v1 + 1;
            size_t v3 = v1 + size;
            adjacency_list[v1]->neighbors.push_front(v2);
            adjacency_list[v1]->neighbors.push_front(v3);
            adjacency_list[v1]->degree = 4;
            adjacency_list[v2]->neighbors.push_front(v1);
            adjacency_list[v3]->neighbors.push_front(v1);
        }
    }
    for (size_t col = size - 2; col < SIZE_MAX; col--) {
        size_t v1 = (size - 1) * size + col;
        size_t v2 = v1 + 1;
        size_t v3 = col;
        adjacency_list[v1]->neighbors.push_front(v2);
        adjacency_list[v1]->neighbors.push_front(v3);
        adjacency_list[v1]->degree = 4;
        adjacency_list[v2]->neighbors.push_front(v1);
        adjacency_list[v3]->neighbors.push_front(v1);
    }
    for (size_t row = size - 2; row < SIZE_MAX; row--) {
        size_t v1 = row * size + size - 1;
        size_t v2 = row * size;
        size_t v3 = v1 + size;
        adjacency_list[v1]->neighbors.push_front(v2);
        adjacency_list[v1]->neighbors.push_front(v3);
        adjacency_list[v1]->degree = 4;
        adjacency_list[v2]->neighbors.push_front(v1);
        adjacency_list[v3]->neighbors.push_front(v1);
    }
    size_t v1 = size * size - 1;
    size_t v2 = (size - 1) * size;
    size_t v3 = size - 1;
    adjacency_list[v1]->neighbors.push_front(v2);
    adjacency_list[v1]->neighbors.push_front(v3);
    adjacency_list[v1]->degree = 4;
    adjacency_list[v2]->neighbors.push_front(v1);
    adjacency_list[v3]->neighbors.push_front(v1);
}

lattice_network::~lattice_network() {
//    cout << "Destructor lattice_network" << endl;
}

BA_network::BA_network(size_t n0, size_t nt, size_t nc) : network(n0 + nt) {
//    cout << "Constructor BA_network" << endl;
    if (SIZE_MAX - n0 < nt) {
        throw runtime_error("Error: Number of vertexes is out of range!");
    }
    if (n0 > nt) {
        throw runtime_error("Error: no is more than nc!");
    }
    
    for (size_t i = 0; i < n0; i ++) {
        for (size_t j = 0; j < i; j++) {
            adjacency_list[i]->neighbors.push_front(j);
            adjacency_list[j]->neighbors.push_front(i);
        }
        adjacency_list[i]->degree = n0 - 1;
    }
    size_t degree_sum = n0 * (n0 - 1);
    
    srand((unsigned)time(NULL));
    for (size_t i = n0; i < vertex_sum; i++) {
        unordered_set<size_t> visited;
        size_t j = 0;
        while (j < nc) {
            size_t threshold = 0;
            size_t rand_num = rand() % degree_sum;
            bool flag = true;
            size_t k = 0;
            while (k < i && flag) {
                threshold += adjacency_list[k]->degree;
                if (rand_num < threshold) {
                    flag = false;
                    if (visited.find(k) == visited.end()) {
                        adjacency_list[i]->neighbors.push_front(k);
                        adjacency_list[i]->degree++;
                        adjacency_list[k]->neighbors.push_front(i);
                        adjacency_list[k]->degree++;
                        visited.insert(k);
                        j++;
                    }
                }
                k++;
            }
        }
        degree_sum += 2 * nc;
    }
}

BA_network::~BA_network() {
//    cout << "Destructor BA_network" << endl;
}
