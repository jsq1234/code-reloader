#include "mylib.hpp"

#include <iostream>

extern "C" {
void hello_world() { std::cout << "Hellya\n"; }
}
