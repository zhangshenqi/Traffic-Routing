//
//  test.cpp
//  Traffic-Routing
//
//  Created by 张沈麒 on 2/6/20.
//  Copyright © 2020 张沈麒. All rights reserved.
//

#include "test.hpp"
#include <iostream>
using namespace std;

test::test(network *net, RoutingMode mode_type, size_t packet_generating_rate, size_t packet_processing_rate, size_t time_length, size_t sample_time_1, size_t sample_time_2) {
    this->net = net;
    switch (mode_type) {
        case shortest_path_static:
            mode = new shortest_path_static_routing_mode(net);
            break;
        case efficient_path_static:
            mode = new efficient_path_static_routing_mode(net);
            break;
        case quickest_path_static:
            mode = new quickest_path_static_routing_mode(net);
            break;
        case quickest_path_dynamic:
            mode = new quickest_path_dynamic_routing_mode(net);
            break;
        default:
            throw runtime_error("Error: Mode type is invalid!");
    }
    
    this->packet_generating_rate = packet_generating_rate;
    this->packet_processing_rate = packet_processing_rate;
    
    if (time_length <= sample_time_1 || time_length <= sample_time_2) {
        throw runtime_error("Error: sample_time is out of range!");
    }
    
    if (sample_time_1 >= sample_time_2) {
        throw runtime_error("Error: sample time is not in descending order.");
    }
    
    this->time_length = time_length;
    this->sample_time_1 = sample_time_1;
    this->sample_time_2 = sample_time_2;
}

double test::get_routing_capacity() {
    size_t packet_sum = 0;
    size_t packet_sum_1 = 0, packet_sum_2 = 0;
    size_t vertex_sum = net->get_vertex_sum();
    for (size_t time = 0; time < time_length; time++) {
        // Process packets.
        for (size_t i = 0; i < vertex_sum; i++) {
            size_t sum = min(packet_processing_rate, net->get_packet_queue_size(i));
            for (size_t j = 0; j < sum; j++) {
                packet *ptr = net->poll_packet_queue(i);
                if (ptr->has_next()) {
                    net->push_packet_queue(ptr->next(), ptr);
                } else {
                    delete ptr;
                    packet_sum--;
                }
            }
        }
        
        // Generate packets.
        for (size_t i = 0; i < packet_generating_rate; i++) {
            packet *ptr = mode->generate_packet();
            net->push_packet_queue(ptr->get_source(), ptr);
        }
        packet_sum += packet_generating_rate;
        
        if (time == sample_time_1) {
            packet_sum_1 = packet_sum;
        } else if (time == sample_time_2) {
            packet_sum_2 = packet_sum;
        }
        
        cout << "time: " << time << " " << "packet_sum: " << packet_sum << endl;
    }
    
    return (double)packet_processing_rate / (double)packet_generating_rate * ((double)packet_sum_2 - (double)packet_sum_1) / ((double)sample_time_2 - (double)sample_time_1);
}


