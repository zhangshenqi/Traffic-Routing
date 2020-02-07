//
//  main.cpp
//  Traffic-Routing
//
//  Created by 张沈麒 on 1/28/20.
//  Copyright © 2020 张沈麒. All rights reserved.
//

#include <iostream>
#include "network.hpp"
#include "routing_mode.hpp"
#include "test.hpp"
using namespace std;

int main(int argc, const char * argv[]) {
//    network *net = new BA_network(5, 1220, 2);
    
//    network *net = new network(10, {{0, 1},{0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9}, {1, 2}, {2, 3}, {3, 4}, {4, 5}});
//    net->print();
//    vector<unsigned> path = net->get_efficient_path(5, 2);
//    for (unsigned vertex: path) {
//        cout << vertex << " ";
//    }
//    cout << endl;
    
//    routing_mode *mode = new shortest_path_static_routing_mode(*net);
//    for (int i = 0; i < 10; i++) {
//        packet *p = mode->generate_packet();
//        p->print();
//        delete p;
//    }
//    delete net;
    
    network *net = new BA_network(5, 1220, 2);
//    net->print();
//    test *t = new test(net, shortest_path_static, 50, 3, 1000, 799, 999);
//    test *t = new test(net, efficient_path_static, 50, 3, 1000, 799, 999);
    test *t = new test(net, quickest_path_static, 50, 3, 1000, 799, 999);
//    test *t = new test(net, quickest_path_dynamic, 50, 1, 1000, 799, 999);
    double capacity = t->get_routing_capacity();
    cout << "routing capacity: " << capacity << endl;
    delete t;
    delete net;
    return 0;
}
