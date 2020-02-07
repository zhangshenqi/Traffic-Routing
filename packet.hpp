//
//  packet.hpp
//  Traffic-Routing
//
//  Created by 张沈麒 on 2/6/20.
//  Copyright © 2020 张沈麒. All rights reserved.
//

#ifndef packet_h
#define packet_h

#include <vector>
//#include "network.hpp"
using namespace std;

class network;

class packet {
public:
    packet();
    virtual ~packet();
    virtual size_t get_source() = 0;
    virtual bool has_next() = 0;
    virtual size_t next() = 0;
    virtual void print() = 0;
};

class packet_static_routing : public packet {
public:
    packet_static_routing(vector<size_t> path);
    ~packet_static_routing();
    size_t get_source();
    bool has_next();
    size_t next();
    void print();
private:
    vector<size_t> path;
    size_t index;
};

class packet_dynamic_routing : public packet {
public:
    packet_dynamic_routing(size_t source, size_t destination, network *net);
    ~packet_dynamic_routing();
    size_t get_source();
    bool has_next();
    size_t next();
    void print();
private:
    size_t destination;
    size_t curr;
    network *net;
};

#endif /* packet_h */
