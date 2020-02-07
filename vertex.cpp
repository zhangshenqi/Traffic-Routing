//
//  vertex.cpp
//  Traffic-Routing
//
//  Created by 张沈麒 on 1/29/20.
//  Copyright © 2020 张沈麒. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "vertex.hpp"
using namespace std;

void vertex::print() {
    cout << "degree: " << degree << "\t";
    cout << "neighbors:";
    for (size_t neighbor: neighbors) {
        cout << " " << neighbor;
    }
    cout << endl;
}
