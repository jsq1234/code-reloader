#include <exception>
#include <iostream>
#include <moniter.hpp>
#include <reloader.hpp>
#include <test.hpp>

int main() {
    std::string src_dir = "/home/manan/programs/code-reloader/src";
    Monitor monitor = Monitor(src_dir, {
                                           "/test/test.cpp",
                                       });
    monitor.runLoop();
}