//
//  routing_mode.hpp
//  Traffic-Routing
//
//  Created by 张沈麒 on 2/6/20.
//  Copyright © 2020 张沈麒. All rights reserved.
//

#ifndef routing_mode_h
#define routing_mode_h

#include <cstdlib>
#include <ctime>
#include "network.hpp"
#include "packet.hpp"

enum RoutingMode {shortest_path_static, efficient_path_static, quickest_path_static, quickest_path_dynamic};

class routing_mode {
public:
    routing_mode(network *net);
    ~routing_mode();
    virtual packet *generate_packet() = 0;
protected:
    network *net;
};

class shortest_path_static_routing_mode : public routing_mode {
public:
    shortest_path_static_routing_mode(network *net);
    ~shortest_path_static_routing_mode();
    packet *generate_packet();
};

class efficient_path_static_routing_mode : public routing_mode {
public:
    efficient_path_static_routing_mode(network *net);
    ~efficient_path_static_routing_mode();
    packet *generate_packet();
};

class quickest_path_static_routing_mode : public routing_mode {
public:
    quickest_path_static_routing_mode(network *net);
    ~quickest_path_static_routing_mode();
    packet *generate_packet();
};

class quickest_path_dynamic_routing_mode : public routing_mode {
public:
    quickest_path_dynamic_routing_mode(network *net);
    ~quickest_path_dynamic_routing_mode();
    packet *generate_packet();
};

#endif /* routing_mode_h */
