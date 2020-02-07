//
//  test.hpp
//  Traffic-Routing
//
//  Created by 张沈麒 on 2/6/20.
//  Copyright © 2020 张沈麒. All rights reserved.
//

#ifndef test_hpp
#define test_hpp

#include <stdio.h>
#include "network.hpp"
#include "routing_mode.hpp"
#include "packet.hpp"

class test {
public:
    test(network *net, RoutingMode mode_type, size_t packet_generating_rate, size_t packet_processing_rate, size_t time_length, size_t sample_time_1, size_t sample_time_2);
    double get_routing_capacity();
protected:
    network *net;
    routing_mode *mode;
    size_t packet_generating_rate;
    size_t packet_processing_rate;
    size_t time_length;
    size_t sample_time_1;
    size_t sample_time_2;
};

#endif /* test_hpp */
