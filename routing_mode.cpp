//
//  routing_mode.cpp
//  Traffic-Routing
//
//  Created by 张沈麒 on 2/6/20.
//  Copyright © 2020 张沈麒. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "routing_mode.hpp"
using namespace std;

routing_mode::routing_mode(network *net) {
//    cout << "Constructor routing_mode" << endl;
    this->net = net;
    srand((unsigned)time(NULL));
}

routing_mode::~routing_mode() {
//    cout << "Destructor routing_mode" << endl;
}

shortest_path_static_routing_mode::shortest_path_static_routing_mode(network *net) : routing_mode(net) {
//    cout << "Constructor shortest_path_static_routing_mode" << endl;
}

shortest_path_static_routing_mode::~shortest_path_static_routing_mode() {
//    cout << "Destructor shortest_path_static_routing_mode" << endl;
}

packet *shortest_path_static_routing_mode::generate_packet() {
    size_t source = rand() % net->get_vertex_sum();
    size_t destination = rand() % net->get_vertex_sum();
    vector<size_t> path = net->get_shortest_path(source, destination);
    return new packet_static_routing(path);
}

efficient_path_static_routing_mode::efficient_path_static_routing_mode(network *net) : routing_mode(net) {
//    cout << "Constructor efficient_path_static_routing_mode" << endl;
}

efficient_path_static_routing_mode::~efficient_path_static_routing_mode() {
//    cout << "Destructor efficient_path_static_routing_mode" << endl;
}

packet *efficient_path_static_routing_mode::generate_packet() {
    size_t source = rand() % net->get_vertex_sum();
    size_t destination = rand() % net->get_vertex_sum();
    vector<size_t> path = net->get_efficient_path(source, destination);
    return new packet_static_routing(path);
}

quickest_path_static_routing_mode::quickest_path_static_routing_mode(network *net) : routing_mode(net) {
    //    cout << "Constructor quickest_path_static_routing_mode" << endl;
}

quickest_path_static_routing_mode::~quickest_path_static_routing_mode() {
    //    cout << "Destructor quickest_path_static_routing_mode" << endl;
}

packet *quickest_path_static_routing_mode::generate_packet() {
    size_t source = rand() % net->get_vertex_sum();
    size_t destination = rand() % net->get_vertex_sum();
    vector<size_t> path = net->get_quickest_path(source, destination);
    return new packet_static_routing(path);
}

quickest_path_dynamic_routing_mode::quickest_path_dynamic_routing_mode(network *net) : routing_mode(net) {
    //    cout << "Constructor quickest_path_dynamic_routing_mode" << endl;
}

quickest_path_dynamic_routing_mode::~quickest_path_dynamic_routing_mode() {
    //    cout << "Destructor quickest_path_dynamic_routing_mode" << endl;
}

packet *quickest_path_dynamic_routing_mode::generate_packet() {
    size_t source = rand() % net->get_vertex_sum();
    size_t destination = rand() % net->get_vertex_sum();
    return new packet_dynamic_routing(source, destination, net);
}
