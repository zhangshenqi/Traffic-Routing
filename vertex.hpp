//
//  vertex.hpp
//  Traffic-Routing
//
//  Created by 张沈麒 on 1/28/20.
//  Copyright © 2020 张沈麒. All rights reserved.
//

#ifndef vertex_h
#define vertex_h

#include <forward_list>
#include <queue>
#include "packet.hpp"
using namespace std;

class vertex {
public:
    forward_list<size_t> neighbors;
    size_t degree;
    queue<packet *> packet_queue;
    void print();
};

#endif /* vertex_h */
