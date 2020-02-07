//
//  packet.cpp
//  Traffic-Routing
//
//  Created by 张沈麒 on 2/6/20.
//  Copyright © 2020 张沈麒. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include "packet.hpp"
#include "network.hpp"
using namespace std;

packet::packet() {
//    cout << "Constructor packet" << endl;
}

packet::~packet() {
//    cout << "Destructor packet" << endl;
}

packet_static_routing::packet_static_routing(vector<size_t> path) : packet() {
//    cout << "Constructor packet_static_routing" << endl;
    this->path = path;
    index = 0;
}

packet_static_routing::~packet_static_routing() {
//    cout << "Destructor packet_static_routing" << endl;
}

size_t packet_static_routing::get_source() {
    index = 1;
    return path[0];
}

bool packet_static_routing::has_next() {
    return index < path.size();
}

size_t packet_static_routing::next() {
    size_t v = path[index];
    index++;
    return v;
}

void packet_static_routing::print() {
    cout << "path:";
    for (size_t vertex: path) {
        cout << " " << vertex;
    }
    cout << endl;
}

packet_dynamic_routing::packet_dynamic_routing(size_t source, size_t destination, network *net) {
//    cout << "Constructor packet_dynamic_routing" << endl;
    this->curr = source;
    this->destination = destination;
    this->net = net;
}

packet_dynamic_routing::~packet_dynamic_routing() {
//    cout << "Destructor packet_dynamic_routing" << endl;
}

size_t packet_dynamic_routing::get_source() {
    return curr;
}

bool packet_dynamic_routing::has_next() {
    return curr != destination;
}

size_t packet_dynamic_routing::next() {
    vector<size_t> path = net->get_quickest_path(curr, destination);
    curr = path[1];
//    cout << "path:";
//    if (curr > net->get_vertex_sum()) {
//        for (size_t v: path) {
//            cout << " " << v;
//        }
//        cout << endl;
//        this->print();
//    }
    return curr;
}

void packet_dynamic_routing::print() {
    cout << "destination: " << destination << endl;
    cout << "curr: " << curr << endl;
}
