#include "test.hpp"

#include <cstdio>
#include <iostream>

void foo() { std::cout << "Yum\n"; }
int bar = 66;

void run() {
    for (int j = 0; j < 10; j++) {
        foo();
    }
    std::cout << "Bar = " << bar << std::endl;
}